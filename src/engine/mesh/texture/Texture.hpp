#pragma once

#include "image2D.hpp"

class Texture {
public:
  Texture();

  Texture(
    const fspath& path,
    const std::string& uniform,
    const GLuint& unit = 0,
    const GLenum& target = GL_TEXTURE_2D,
    const GLint& internalFormat = 0,
    const GLenum& format = 0,
    const GLenum& type = GL_UNSIGNED_BYTE
  );

  Texture(
    const fspath& path1,
    const fspath& path2,
    const std::string& uniform,
    const GLuint& unit,
    const GLenum& target,
    const GLint& internalFormat,
    const GLenum& format,
    const GLenum& type
  );

  void bind() const;
  void unbind() const;
  void clear();

  const GLenum& getType() const;
  const GLuint& getUnit() const;
  const std::string& getUniformName() const;
  const uvec3& getSize() const;

private:
  const GLenum target;
  const GLint internalFormat;
  const GLenum format;
  const GLenum type;
  const std::string uniformName;
  const GLuint unit;

  GLuint id;
  uvec3 size;

private:
  void create2D(const image2D& img);
  void create2DArray(const image2D& img0, const image2D& img1);
};

