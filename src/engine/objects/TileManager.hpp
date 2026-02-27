#pragma once

#include "../texture/TextureVirtual.hpp"
#include "../PingPongBuffer.hpp"

class TileManager {
public:
  TileManager() = default;
  TileManager(const TextureVirtual::Capabilities& caps);
  ~TileManager();

  void addTexture(const TextureVirtual* tex);

  void requestTile(vec2 uvMin, vec2 uvMax);
  void processRequests();

protected:
  friend struct gui;

  struct PhysicalSlot {
    int virtualIdx = -1;
    float timestamp = 0.f;
  };

  struct Request {
    bool isUsing = false;
    ivec2 tileCoord;
    int slot;
  };

  struct TexData {
    const TextureVirtual* tex;
    VipsRegion* region;
    size_t pelSize;
    GLenum physicalTexFormat;
  };

  TextureVirtual::Capabilities caps;
  PingPongBuffer ppBuffer;

  std::vector<PhysicalSlot> physicalSlots;
  std::vector<Request> requests;
  std::list<TexData> texs;

  int tilesLoaded = 0;

protected:
  static GLenum getGLformat(VipsBandFormat f);

  int getTileSlot(int virtualIdx) const;
  int getFirstAvailableSlot() const;
  VipsRect prepareArea(VipsRegion* img, ivec2 coord) const;

  void uploadPhysical(const TexData& texData, ivec2 coord, int slot);
  void uploadIndirection(const TexData& tecData, ivec2 coord, int slot);
};

