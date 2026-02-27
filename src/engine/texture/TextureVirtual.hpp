#pragma once

#include "Texture2D.hpp"
#include "Texture2DArray.hpp"

class TextureVirtual {
public:
  struct Capabilities {
    int tileSlots;
    ivec2 tileSize;
    ivec2 virtualDims;
  };

  static Texture2D generateIndirection(const TextureDescriptor& desc, const Capabilities& caps);

  TextureVirtual() = default;
  TextureVirtual(const fspath &filePath, const TextureDescriptor &desc, const Capabilities &caps, Texture2D *indirection);

  TextureVirtual(const TextureVirtual&) = delete;
  TextureVirtual& operator=(const TextureVirtual&) = delete;

  TextureVirtual(TextureVirtual&&) = default;
  TextureVirtual& operator=(TextureVirtual&&) = default;

  const Texture2DArray& getPhysical() const;
  const Texture2D* getIndirection() const;
  const vips::VImage& getImage() const;

  void bind(ivec2 slotsPhyInd);
  void bind();

private:
  Texture2D* texIndirection;
  Texture2DArray texPhysical;
  vips::VImage image;
};

