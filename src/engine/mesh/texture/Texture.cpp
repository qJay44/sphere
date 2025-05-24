#include "Texture.hpp"

#include <cassert>
#include <format>

constexpr GLenum GL_STANDARD_CHANNELS[4] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};

Texture::Texture()
  : uniformName(),
    unit(),
    target(),
    internalFormat(),
    format(),
    type() {}

Texture::Texture(
  const fspath& path,
  const std::string& uniform,
  const GLuint& unit,
  const GLenum& target,
  const GLint& internalFormat,
  const GLenum& format,
  const GLenum& type
)
  : uniformName(uniform),
    unit(unit),
    target(target),
    internalFormat(internalFormat),
    format(format),
    type(type)
{
  switch (target) {
    case GL_TEXTURE_2D:
      create2D(image2D(path));
      break;
    default:
      error(std::format("Unhandled texture creation type: [{}]", target));
      break;
  }
  unbind();
}

Texture::Texture(
  const fspath& path1,
  const fspath& path2,
  const std::string& uniform,
  const GLuint& unit,
  const GLenum& target,
  const GLint& internalFormat,
  const GLenum& format,
  const GLenum& type
)
  : uniformName(uniform),
    unit(unit),
    target(target),
    internalFormat(internalFormat),
    format(format),
    type(type)
{
  switch (target) {
    case GL_TEXTURE_2D_ARRAY:
      create2DArray(image2D(path1), image2D(path2));
      break;
    default:
      error(std::format("Unhandled texture creation type: [{}]", target));
      break;
  }
  unbind();
}

Texture::Texture(const Texture& other) :
  target(other.target),
  internalFormat(other.internalFormat),
  format(other.format),
  type(other.type),
  uniformName(other.uniformName),
  unit(other.unit),
  id(other.id),
  size(other.size) {}

Texture Texture::operator=(const Texture& other) {
  return Texture(other);
}

void Texture::bind() const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(target, id);
}

void Texture::unbind() const {
  glBindTexture(target, 0); //
}

void Texture::clear() {
  glDeleteTextures(1, &id); //
}

const GLenum& Texture::getType() const { return target; }
const GLuint& Texture::getUnit() const { return unit; }
const std::string& Texture::getUniformName() const { return uniformName; }
const uvec3& Texture::getSize() const { return size; }

void Texture::create2D(const image2D& img) {
  const GLenum& texFormat = internalFormat ? internalFormat : GL_STANDARD_CHANNELS[img.channels - 1];
  const GLenum& imgFormat = format ? format : GL_STANDARD_CHANNELS[img.channels - 1];

  size = {img.width, img.height, 1};
  glGenTextures(1, &id);
  bind();
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);

  status::start("Creating texture", img.name);
  glTexImage2D(target, 0, texFormat, img.width, img.height, 0, imgFormat, type, img.pixels);
  status::end(true);
}

void Texture::create2DArray(const image2D& img0, const image2D& img1) {
  assert(img0.width == img1.width);
  assert(img0.height == img1.height);
  assert(img0.channels == img1.channels);

  size = {img0.width, img0.height, 2};
  glGenTextures(1, &id);
  bind();
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexStorage3D(target, 1, internalFormat, size.x, size.y, 2);

  status::start("Creating texture", img0.name);
  glTexSubImage3D(target, 0, 0, 0, 0, size.x, size.y, 1, format, type, img0.pixels);
  status::end(true);

  status::start("Creating texture", img1.name);
  glTexSubImage3D(target, 0, 0, 0, 1, size.x, size.y, 1, format, type, img1.pixels);
  status::end(true);
}

