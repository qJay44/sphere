#pragma once

#include "TextureDescriptor.hpp"

class Texture {
public:
  Texture(Texture&& other);
  Texture& operator=(Texture&& other);

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  virtual ~Texture() = 0;

  void bind(GLuint unit) const;
  void unbind() const;
  void clear();

  const GLuint& getId() const;
  const GLenum& getTarget() const;

  ivec2 getSize(GLint mipLevel) const;

protected:
  TextureDescriptor desc{};
  GLuint id = 0;

protected:
  Texture() = default;
  Texture(const TextureDescriptor& desc);
};

