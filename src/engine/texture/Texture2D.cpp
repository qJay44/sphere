#include "Texture2D.hpp"

Texture2D Texture2D::debugTex0;

const Texture2D& Texture2D::getDebugTex0() {
  if (debugTex0.id == 0)
    debugTex0 = Texture2D("res/tex/debug/uvChecker.jpg", {
      .minFilter = GL_NEAREST,
      .magFilter = GL_NEAREST,
    });

  return debugTex0;
}

Texture2D Texture2D::storage(const ivec2& size, const TextureDescriptor& desc, GLsizei levels) {
  Texture2D tex;

  tex.onInit(desc);
  glTexStorage2D(tex.desc.target, levels, desc.internalFormat, size.x, size.y);

  if (desc.genMipMap)
    glGenerateMipmap(desc.target);

  tex.unbind();

  return tex;
}

Texture2D::Texture2D(const image2D& img, const TextureDescriptor& d) : Texture(d) {
  if (desc.target != GL_TEXTURE_2D)
    error("[Texture2D::Texture2D] Wrong tartget [{:#x}]", desc.target);

  glGenTextures(1, &id);
  bind(0);
  glTexParameteri(desc.target, GL_TEXTURE_MIN_FILTER, desc.minFilter);
  glTexParameteri(desc.target, GL_TEXTURE_MAG_FILTER, desc.magFilter);
  glTexParameteri(desc.target, GL_TEXTURE_WRAP_S, desc.wrapS);
  glTexParameteri(desc.target, GL_TEXTURE_WRAP_T, desc.wrapT);
  glTexImage2D(desc.target, 0, desc.internalFormat, img.width, img.height, 0, desc.format, desc.type, img.pixels);

  if (desc.genMipMap)
    glGenerateMipmap(desc.target);

  unbind();
}

Texture2D::Texture2D(const ivec2& size, const TextureDescriptor& desc)
  : Texture2D(image2D{size.x, size.y}, desc) {}

Texture2D::Texture2D(const fspath& path, const TextureDescriptor& desc)
  : Texture2D(image2D(path), desc) {}

void Texture2D::upload(ivec2 coord, ivec2 size, const void* data, GLenum type) const {
  bind(0);
  glTexSubImage2D(desc.target, 0, coord.x, coord.y, size.x, size.y, desc.format, type, data);
  unbind();
}

void Texture2D::onInit(const TextureDescriptor& desc) {
  this->desc.target = desc.target;

  if (desc.target != GL_TEXTURE_2D)
    error("[Texture2D::Texture2D] Wrong target ({:#x})", desc.target);

  glGenTextures(1, &id);
  bind(0);
  glTexParameteri(desc.target, GL_TEXTURE_MIN_FILTER, desc.minFilter);
  glTexParameteri(desc.target, GL_TEXTURE_MAG_FILTER, desc.magFilter);
  glTexParameteri(desc.target, GL_TEXTURE_WRAP_S, desc.wrapS);
  glTexParameteri(desc.target, GL_TEXTURE_WRAP_T, desc.wrapT);
}

