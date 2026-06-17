#pragma once

#include "../texture/TextureVirtual.hpp"
#include "../mesh/BufferObject.hpp"
#include <functional>
#include <queue>

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
    ivec2 tileCoord{};
    int slot = -1;
  };

  struct TexData {
    const TextureVirtual* tex;
    VipsRegion* region;
    size_t pelSize;
    GLenum physicalTexFormat;
  };

  struct PBO {
    BufferObject native;
    void* persistentPtr;
    bool isAvailable;
    GLsync fence;
  };

  struct LayerTask {
    TexData texData;
    size_t pboByteOffset;
  };

  TextureVirtual::Capabilities caps;
  size_t tileSizeInBytes;
  size_t tileSizeInBytesMax;
  std::vector<PhysicalSlot> physicalSlots;
  std::vector<Request> requests;

  // Need a lot only at the app launch (chunks per axis dependent)
  static constexpr int pboPoolSize = 32;
  PBO pboPool[pboPoolSize];

  std::queue<std::function<void()>> mainThreadQueue;
  std::mutex queueMutex;

  std::list<TexData> texs;

  int tilesLoaded = 0;

protected:
  static GLenum getGLformat(VipsBandFormat f);

  int getTileSlot(int virtualIdx) const;
  int getFirstAvailableSlot() const;

  VipsRect prepareArea(VipsRegion* img, ivec2 coord) const;

  int leaseAvailablePBO();
  void uploadTextures(std::vector<LayerTask>&& layerTasks, ivec2 coord, int slot);

  void enqueMainThreadUpload(std::function<void()> task);
  void processMainThreadUploads();
};

