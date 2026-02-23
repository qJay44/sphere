#include "TileManager.hpp"
#include "utils/utils.hpp"
#include <map>

TileManager::TileManager(int tileSlots, ivec2 tileSize, ivec2 virtualTileSize)
  : tileSlots(tileSlots),
    tileSize(tileSize),
    virtualTileSize(virtualTileSize)
{
  physicalSlots.resize(tileSlots);
  // requests.resize(virtualTileSize.x * virtualTileSize.y);
}

void TileManager::requestTile(vec2 uvMin, vec2 uvMax) {
  ivec2 tileStart = floor(uvMin * vec2(virtualTileSize));
  ivec2 tileEnd = floor(uvMax * vec2(virtualTileSize));

  for (int ty = tileStart.y; ty < tileEnd.y; ty++)
    for (int tx = tileStart.x; tx < tileEnd.x; tx++) {
      int virtualIdx = ty * virtualTileSize.x + tx;
      int slot = getTileSlot(virtualIdx);
      requests.push({{tx, ty}, slot, virtualIdx});
    }
}

void TileManager::addTexture(const TextureVirtual* tex) {
  texs.push_back(tex);
}

void TileManager::processRequests() {
  tilesLoaded = 0;

  while (!requests.empty()) {
    const Request& r = requests.top();
    int slot = r.slot;

    if (slot == -1) {
      slot = getFirstAvailableSlot();
      physicalSlots[slot].virtualIdx = r.virtualIdx;

      u8 slotByte = slot;

      // TODO: Add PBO
      for (const TextureVirtual* tex : texs) {
        auto pixelData = getPixels(tex->getImage(), r.tileCoord);

        tex->getPhysical().upload(slot, pixelData.pixels, getGLformat(pixelData.vipsFormat));
        tex->getIndirection().upload(r.tileCoord, {1, 1}, &slotByte, GL_UNSIGNED_BYTE);

        g_free(pixelData.pixels);
      }

      tilesLoaded++;
    }

    physicalSlots[slot].timestamp = global::time;
    requests.pop();
  }

  if (tilesLoaded > tileSlots - 10)
    warning("[TileManager::processRequests] Tiles loaded: {}/{}", tilesLoaded, tileSlots);
}

GLenum TileManager::getGLformat(VipsBandFormat f) {
  static const std::map<VipsBandFormat, GLenum> vipsToGL = {
    {VIPS_FORMAT_UCHAR, GL_UNSIGNED_BYTE},
    {VIPS_FORMAT_USHORT, GL_UNSIGNED_SHORT},
  };

  auto it = vipsToGL.find(f);
  if (it == vipsToGL.end())
    error("[TileManager::getGLformat] No GL format for [{}]", (int)f);

  return it->second;
}

int TileManager::getTileSlot(int virtualIdx) const {
  for (int i = 0; i < tileSlots; i++)
    if (physicalSlots[i].virtualIdx == virtualIdx)
      return i;

  return -1;
}

int TileManager::getFirstAvailableSlot() const {
  int res = 0;
  float oldest = global::time;

  for (int i = 0; i < tileSlots; i++)
    if (physicalSlots[i].timestamp < oldest) {
      res = i;
      oldest = physicalSlots[i].timestamp;
    }

  return res;
}

TileManager::PixelData TileManager::getPixels(vips::VImage img, ivec2 coord) const {
  ivec2 pos = coord * tileSize;

  vips::VImage tile = img.extract_area(pos.x, pos.y, tileSize.x, tileSize.y);

  size_t bufferSize;
  void* buffer = tile.write_to_memory(&bufferSize);

  return {buffer, tile.format()};
}

