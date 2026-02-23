#pragma once

#include <stack>
#include <vector>
#include <vips/vips8>

#include "../texture/TextureVirtual.hpp"

class TileManager {
public:
  TileManager() = default;
  TileManager(int tileSlots, ivec2 tileSize, ivec2 virtualTileSize);

  void addTexture(const TextureVirtual* tex);

  void requestTile(vec2 uvMin, vec2 uvMax);
  void processRequests();

protected:
  friend struct gui; // TODO: Make a plot of history of chunks drawn and tiles loaded

  int tileSlots = 0;
  ivec2 tileSize{0};
  ivec2 virtualTileSize{0};

  struct PhysicalSlot {
    int virtualIdx = -1;
    float timestamp = 0.f;
  };

  struct Request {
    ivec2 tileCoord;
    int slot = 1;
    int virtualIdx = -1;
  };

  struct PixelData {
    void* pixels;
    VipsBandFormat vipsFormat;
  };

  std::vector<PhysicalSlot> physicalSlots;
  std::stack<Request> requests;
  std::list<const TextureVirtual*> texs;

  int tilesLoaded = 0;

protected:
  static GLenum getGLformat(VipsBandFormat f);

  int getTileSlot(int virtualIdx) const;
  int getFirstAvailableSlot() const;
  PixelData getPixels(vips::VImage img, ivec2 coord) const;
};

