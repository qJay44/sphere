#pragma once

#include "image2D.hpp"

class Texture {
public:
  Texture();
  Texture(const image2D& img, GLenum type, std::string uniform, GLuint unit = 0);
  Texture(const fspath& path, GLenum type, std::string uniform, GLuint unit = 0);

  void bind() const;
  void unbind() const;
  void clear();

  const GLenum& getType() const;
  const GLuint& getUnit() const;
  const std::string& getUniformName() const;

private:
  GLuint unit;
  std::string uniformName;

  GLuint id;
  GLenum glType;

private:
  void build2D(const image2D& img);
};

