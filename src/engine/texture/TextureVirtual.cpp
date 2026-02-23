#include "TextureVirtual.hpp"

TextureVirtual::TextureVirtual(const TextureDescriptorVirtual& desc) : desc(desc) {
  std::string uniformNamePrefix = "u_tex" + desc.name;

  texPhysical = Texture2DArray(desc.tileSlots, desc.tileSize, {
    .uniformName    = uniformNamePrefix + "Physical",
    .unit           = desc.unitsPhyInd.x,
    .target         = GL_TEXTURE_2D_ARRAY,
    .internalFormat = desc.internalFormat,
    .format         = desc.format,
    .minFilter      = desc.minFilter,
    .magFilter      = desc.magFilter,
    .wrapS          = GL_CLAMP_TO_EDGE,
    .wrapT          = GL_CLAMP_TO_EDGE,
    .genMipMap      = false,
  });

  texIndirection = Texture2D(desc.virtualDims, {
    .uniformName    = uniformNamePrefix + "Indirection",
    .unit           = desc.unitsPhyInd.y,
    .internalFormat = GL_R8UI,
    .format         = GL_RED_INTEGER,
    .minFilter      = GL_NEAREST,
    .magFilter      = GL_NEAREST,
    .wrapS          = GL_CLAMP_TO_EDGE,
    .wrapT          = GL_CLAMP_TO_EDGE,
    .genMipMap      = false,
  });

  image = vips::VImage::new_from_file(desc.filePath.string().c_str());

  int virtualSlots = desc.virtualDims.x * desc.virtualDims.y;
  texIndirection.fill(std::vector<u8>(virtualSlots, 255).data(), GL_UNSIGNED_BYTE);
}

const Texture2DArray& TextureVirtual::getPhysical() const { return texPhysical; }
const Texture2D& TextureVirtual::getIndirection() const { return texIndirection; }
const vips::VImage& TextureVirtual::getImage() const { return image; }

void TextureVirtual::bind(ivec2 unitsPhyInd) {
  texPhysical.bind(unitsPhyInd.x);
  texIndirection.bind(unitsPhyInd.y);
}

void TextureVirtual::bind() {
  bind(desc.unitsPhyInd);
}

