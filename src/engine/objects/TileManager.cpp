#include "TileManager.hpp"

#include "glib-object.h"
#include "glm/gtx/component_wise.hpp"
#include "global.hpp"
#include "utils/utils.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>
#include <utility>
#include <vips/error.h>
#include <vips/region.h>

TileManager::TileManager(const TextureVirtual::Capabilities& caps)
  : caps(caps),
    tileSizeInBytes(glm::compMul(caps.tileSize) * sizeof(float)),
    tileSizeInBytesMax(glm::compMul(caps.tileSize) * sizeof(float) * 32), // NOTE: A bit more than enough to store the 3 textures
    physicalSlots(caps.tileSlots),
    requests(glm::compMul(caps.virtualDims))
{
  GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

  for (int i = 0; i < pboPoolSize; i++) {
    BufferObject pboNative{GL_PIXEL_UNPACK_BUFFER};
    pboNative.storage(nullptr, tileSizeInBytesMax, flags);
    pboNative.bind();

    PBO pbo{};
    pbo.native = std::move(pboNative);
    pbo.persistentPtr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, tileSizeInBytesMax, flags);
    pbo.fence = nullptr;
    pbo.isAvailable = true;

    if (!pbo.persistentPtr)
      error("[TileManager::TileManager] OpenGL failed to persistently map PBO");

    pboNative.unbind();

    pboPool[i] = std::move(pbo);
  }
}

TileManager::~TileManager() {
  for (TexData& texData : texs)
    g_object_unref(texData.region);
}

void TileManager::requestTile(vec2 uvMin, vec2 uvMax) {
  ivec2 tileStart = floor(uvMin * vec2(caps.virtualDims));
  ivec2 tileEnd = ceil(uvMax * vec2(caps.virtualDims));

  for (int ty = tileStart.y; ty < tileEnd.y; ty++)
    for (int tx = tileStart.x; tx < tileEnd.x; tx++) {
      int virtualIdx = ty * caps.virtualDims.x + tx;
      int slot = getTileSlot(virtualIdx);
      requests[virtualIdx] = {true, {tx, ty}, slot};
    }
}

void TileManager::addTexture(const TextureVirtual* tex) {
  const auto& img = tex->getImage().get_image();
  static size_t currTotalChunkSizeInBytes = 0;

  if (!texs.empty())
    if (texs.front().tex->getIndirection() != tex->getIndirection())
      printf("[TileManager::addTexture] Added textures has different indirection texture\n");

  texs.push_back({
    tex,
    vips_region_new(img),
    VIPS_IMAGE_SIZEOF_PEL(img),
    getGLformat(img->BandFmt)
  });

  currTotalChunkSizeInBytes += tileSizeInBytes * texs.back().pelSize;
  printf("PBO storage space [%zu/%zu] Bytes\n", currTotalChunkSizeInBytes, tileSizeInBytesMax);
}

void TileManager::processRequests() {
  processMainThreadUploads();

  tilesLoaded = 0;

  for (size_t i = 0; i < requests.size(); i++) {
    Request& r = requests[i];

    if (!r.isUsing)
      continue;

    int slot = r.slot;

    if (slot == -1) {
      slot = getFirstAvailableSlot();
      physicalSlots[slot].virtualIdx = i;
      physicalSlots[slot].timestamp = global::time;

      std::vector<LayerTask> layerDatas;
      size_t byteOffset = 0;

      for (TexData texData : texs) {
        layerDatas.push_back({texData, byteOffset});
        byteOffset += tileSizeInBytes * texData.pelSize;
      }

      uploadTextures(std::move(layerDatas), r.tileCoord, slot);

      tilesLoaded++;
    }

    r.isUsing = false;
    r.slot = -1;
  }

  if (tilesLoaded > caps.tileSlots - 10)
    warning("[TileManager::processRequests] Tiles loaded: {}/{}", tilesLoaded, caps.tileSlots);
}

GLenum TileManager::getGLformat(VipsBandFormat vipsFormat) {
  constexpr GLenum unknownFormat = 0xffffffff;

  static const GLenum vipsToGL[12] = {
    unknownFormat,
    GL_UNSIGNED_BYTE,
    GL_BYTE,
    GL_UNSIGNED_SHORT,
    GL_SHORT,
    GL_UNSIGNED_INT,
    GL_INT,
    GL_FLOAT,
    unknownFormat,
    unknownFormat,
    unknownFormat,
    unknownFormat
  };

  GLenum glFormat = vipsToGL[vipsFormat + 1];
  if (glFormat == unknownFormat)
    error("[TileManager::getGLformat] Unknown format [{}]", (int)vipsFormat);

  return glFormat;
}

int TileManager::getTileSlot(int virtualIdx) const {
  for (int i = 0; i < caps.tileSlots; i++)
    if (physicalSlots[i].virtualIdx == virtualIdx)
      return i;

  return -1;
}

int TileManager::getFirstAvailableSlot() const {
  int slot = 0;
  float oldest = global::time;

  for (int i = 0; i < caps.tileSlots; i++)
    if (physicalSlots[i].timestamp < oldest) {
      slot = i;
      oldest = physicalSlots[i].timestamp;
    }

  return slot;
}

int TileManager::leaseAvailablePBO() {
  for (int i = 0; i < pboPoolSize; i++)
    if (std::exchange(pboPool[i].isAvailable, false))
      return i;

  int oldestIdx = 0;
  PBO& pbo = pboPool[oldestIdx];
  if (pbo.fence) {
    glClientWaitSync(pbo.fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
    glDeleteSync(pbo.fence);
    pbo.fence = nullptr;
  }

  return oldestIdx;
}

void TileManager::uploadTextures(std::vector<LayerTask>&& layerTasks, ivec2 coord, int slot) {
  const int leaseIdx = leaseAvailablePBO();
  PBO& pbo = pboPool[leaseIdx];

  VipsRect area;
  area.left = coord.x * caps.tileSize.x;
  area.top = coord.y * caps.tileSize.y;
  area.width = caps.tileSize.x;
  area.height = caps.tileSize.y;

  GLuint pboId = pbo.native.id;
  void* writePtr = pbo.persistentPtr;

  std::thread workerThread([=, this]() {
    bool success = true;

    for (const auto& layer : layerTasks) {
      VipsImage* image = layer.texData.region->im;
      g_object_ref(image);
      VipsRegion* localRegion = vips_region_new(image);

      if (vips_region_prepare(localRegion, const_cast<VipsRect*>(&area)) != 0) {
        g_object_unref(localRegion);
        g_object_unref(image);
        success = false;
        break;
      }

      size_t lineSize = layer.texData.pelSize * area.width;
      auto* dst = (u8*)writePtr + layer.pboByteOffset;

      for (int y = 0; y < area.height; y++) {
        auto* src = (u8*)VIPS_REGION_ADDR(localRegion, area.left, area.top + y);

        assert(dst);
        assert(src);

        memcpy(dst, src, lineSize);

        dst += lineSize;
      }

      g_object_unref(localRegion);
      g_object_unref(image);
    }

    if (!success) {
      this->enqueMainThreadUpload([=, this]() {pboPool[leaseIdx].isAvailable = true;});
      return;
    }

    this->enqueMainThreadUpload([=, this]() {
      glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboId);

      for (const auto& layer : layerTasks) {
        void* offsetPtr = (void*)layer.pboByteOffset;
        layer.texData.tex->getPhysical().upload(slot, offsetPtr, layer.texData.physicalTexFormat);
      }

      PBO& targetPBO = pboPool[leaseIdx];
      targetPBO.fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
      targetPBO.isAvailable = true;

      glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

      u8 slotByte = slot;
      texs.front().tex->getIndirection()->upload(coord, {1, 1}, &slotByte, GL_UNSIGNED_BYTE);
    });
  });

  workerThread.detach();
}

void TileManager::enqueMainThreadUpload(std::function<void()> task) {
  std::lock_guard<std::mutex> lock(queueMutex);
  mainThreadQueue.push(task);
}

void TileManager::processMainThreadUploads() {
  std::vector<std::function<void()>> tasksToRun;

  {
    std::lock_guard<std::mutex> lock(queueMutex);
    while (!mainThreadQueue.empty()) {
      tasksToRun.push_back(std::move(mainThreadQueue.front()));
      mainThreadQueue.pop();
    }
  }

  for (const auto& task : tasksToRun)
    task();
}

