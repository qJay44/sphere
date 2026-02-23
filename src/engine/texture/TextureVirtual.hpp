#pragma once

#include <vips/vips8>

#include "Texture2D.hpp"
#include "Texture2DArray.hpp"
#include "TextureDescriptorVirtual.hpp"

class TextureVirtual {
public:
  TextureVirtual() = default;
  TextureVirtual(const TextureDescriptorVirtual& desc);

  const Texture2DArray& getPhysical() const;
  const Texture2D& getIndirection() const;
  const vips::VImage& getImage() const;

  void bind(ivec2 slotsPhyInd);
  void bind();

private:
  TextureDescriptorVirtual desc;
  Texture2DArray texPhysical;
  Texture2D texIndirection;
  vips::VImage image;
};

