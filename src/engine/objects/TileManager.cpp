#include "TileManager.hpp"

#include "glm/gtx/component_wise.hpp"
#include "global.hpp"

TileManager::TileManager(const TextureVirtual::Capabilities& caps)
  : caps(caps),
    ppBuffer({GL_PIXEL_UNPACK_BUFFER, glm::compMul(caps.tileSize) * 4, GL_STREAM_DRAW}), // NOTE: Up to RGBA8
    physicalSlots(caps.tileSlots),
    requests(glm::compMul(caps.virtualDims))
{
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

  texs.push_back({
    tex,
    vips_region_new(img),
    VIPS_IMAGE_SIZEOF_PEL(img),
    getGLformat(img->BandFmt)
  });
}

void TileManager::processRequests() {
  tilesLoaded = 0;

  for (size_t i = 0; i < requests.size(); i++) {
    Request& r = requests[i];

    if (!r.isUsing)
      continue;

    int slot = r.slot;

    if (slot == -1) {
      slot = getFirstAvailableSlot();
      physicalSlots[slot].virtualIdx = i;

      for (const TexData& texData : texs) {
        uploadPhysical(texData, r.tileCoord, slot);
        uploadIndirection(texData, r.tileCoord, slot);
      }

      tilesLoaded++;
    }

    physicalSlots[slot].timestamp = global::time;
    r.isUsing = false;
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

void TileManager::uploadPhysical(const TexData& texData, ivec2 coord, int slot) {
  auto _task = global::profiler->startScopedTask("TileManager::uploadPhysical", 0xff7f00ff);

  VipsRect area;
  area.left = coord.x * caps.tileSize.x;
  area.top = coord.y * caps.tileSize.y;
  area.width = caps.tileSize.x;
  area.height = caps.tileSize.y;

  if (vips_region_prepare(texData.region, &area))
    error("[TileManager::getTileData] Vips region error [{}]", vips_error_buffer());

  ppBuffer.bind();

  size_t lineSize = texData.pelSize * area.width;
  void* pboPtr = ppBuffer.map(lineSize * area.height);
  u8* dest = (u8*)pboPtr;

  for (int y = 0; y < area.height; y++) {
    void* src = VIPS_REGION_ADDR(texData.region, area.left, area.top + y);
    memcpy(dest, src, lineSize);
    dest += lineSize;
  }

  ppBuffer.unmap();

  texData.tex->getPhysical().upload(slot, (void*)0, texData.physicalTexFormat);

  ppBuffer.unbind();
  ppBuffer.next();
}

void TileManager::uploadIndirection(const TexData& texData, ivec2 coord, int slot) {
  u8 slotByte = slot;
  texData.tex->getIndirection()->upload(coord, {1, 1}, &slotByte, GL_UNSIGNED_BYTE);
}

