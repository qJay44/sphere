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
    const fspath& path0,         // First part
    const fspath& path1,         // Second part
    const std::string& uniform,  // Uniform name in shader
    const GLuint& unit,          // Texture slot
    const GLenum& target,        // Texture type
    const GLint& internalFormat, // Color format in the OpenGL program
    const GLenum& format,        // Color format of the given image(s)
    const GLenum& type           // Color bytes format of the given image(s)
  );

  Texture operator=(const Texture& other);

  void bind() const;
  void unbind() const;
  void clear();

  const GLenum& getType() const;
  const GLuint& getUnit() const;
  const std::string& getUniformName() const;
  const uvec3& getSize() const;

private:
  const std::string uniformName;
  const GLuint unit;
  const GLenum target;
  const GLint internalFormat;
  const GLenum format;
  const GLenum type;

  GLuint id;
  uvec3 size;

private:
  Texture(const Texture& other);

  void create2D(const image2D& img);
  void create2DArray(const image2D& img0, const image2D& img1);
  void createCubemap(const fspath& folder);
};

