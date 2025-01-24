#pragma once

#include "TextureType.hpp"

class Texture {
public:
  Texture();
  Texture(const fspath& path, TextureType type);

  void bind() const;
  void unbind() const;
  void clear();

  const TextureType& getType() const;
  const GLuint& getUnit() const;

private:
  GLuint id;
  GLuint unit;
  GLenum glType;
  TextureType type;
  fspath name;
};

