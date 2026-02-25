#include "TextureVirtual.hpp"

#include "glm/gtx/component_wise.hpp"

Texture2D TextureVirtual::generateIndirection(const TextureDescriptor& desc, const Capabilities& caps) {
  auto tex =  Texture2D(caps.virtualDims, {
    .uniformName    = desc.uniformName,
    .unit           = desc.unit,
    .internalFormat = GL_R8UI,
    .format         = GL_RED_INTEGER,
    .minFilter      = GL_NEAREST,
    .magFilter      = GL_NEAREST,
    .wrapS          = GL_CLAMP_TO_EDGE,
    .wrapT          = GL_CLAMP_TO_EDGE,
    .genMipMap      = false,
  });

  std::vector<u8> white(glm::compMul(caps.virtualDims), 255);
  tex.upload({0, 0}, caps.virtualDims, white.data(), GL_UNSIGNED_BYTE);

  return tex;
}

TextureVirtual::TextureVirtual(const fspath& filePath, const TextureDescriptor& desc, const Capabilities& caps, Texture2D* indirection)
  : texIndirection(indirection)
{
  texPhysical = Texture2DArray(caps.tileSlots, caps.tileSize, {
    .uniformName    = desc.uniformName,
    .unit           = desc.unit,
    .target         = GL_TEXTURE_2D_ARRAY,
    .internalFormat = desc.internalFormat,
    .format         = desc.format,
    .minFilter      = desc.minFilter,
    .magFilter      = desc.magFilter,
    .wrapS          = GL_CLAMP_TO_EDGE,
    .wrapT          = GL_CLAMP_TO_EDGE,
    .genMipMap      = false,
  });

  image = vips::VImage::new_from_file(filePath.string().c_str(),
    vips::VImage::option()->set("access", VIPS_ACCESS_RANDOM));

  image = image.tilecache(vips::VImage::option()
    ->set("tile_width", caps.tileSize.x)
    ->set("tile_height", caps.tileSize.y)
    ->set("max_tiles", caps.tileSlots));
}

const Texture2DArray& TextureVirtual::getPhysical() const { return texPhysical; }
const Texture2D* TextureVirtual::getIndirection() const { return texIndirection; }
const vips::VImage& TextureVirtual::getImage() const { return image; }

void TextureVirtual::bind() {
  texPhysical.bind();
  texIndirection->bind();
}

