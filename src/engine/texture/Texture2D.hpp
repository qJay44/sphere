#pragma once

#include "Texture.hpp"

#include "image2D.hpp"

class Texture2D : public Texture {
public:
  static const Texture2D& getDebugTex0();

  static Texture2D storage(const ivec2& size, const TextureDescriptor& desc, GLsizei levels = 1);

  Texture2D() = default;

  Texture2D(const image2D& img, const TextureDescriptor& desc);
  Texture2D(const ivec2& size , const TextureDescriptor& desc);
  Texture2D(const fspath& path, const TextureDescriptor& desc);

  void upload(ivec2 coord, ivec2 size, const void* data, GLenum type) const;
  void onInit(const TextureDescriptor& desc);

private:
  static Texture2D debugTex0;
};

