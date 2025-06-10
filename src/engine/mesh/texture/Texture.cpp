#include "Texture.hpp"
#include "image2D.hpp"

#include <cassert>
#include <format>
#include <windows.h>

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
      create2D(image2D(path, true));
      break;
    case GL_TEXTURE_CUBE_MAP:
      createCubemap(path);
      break;
    default:
      error(std::format("Unhandled texture creation type: [{}]", target));
      break;
  }
  unbind();
}

Texture::Texture(
  const fspath& path0,
  const fspath& path1,
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
      create2DArray(image2D(path0, true), image2D(path1, true));
      break;
    default:
      error(std::format("Unhandled texture creation type: [{}]", target));
      break;
  }
  unbind();
}

Texture::Texture(const Texture& other) :
  uniformName(other.uniformName),
  unit(other.unit),
  target(other.target),
  internalFormat(other.internalFormat),
  format(other.format),
  type(other.type),
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
  glTexImage2D(target, 0, texFormat, img.width, img.height, 0, imgFormat, type, img.pixels);
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
  glTexSubImage3D(target, 0, 0, 0, 0, size.x, size.y, 1, format, type, img0.pixels);
  glTexSubImage3D(target, 0, 0, 0, 1, size.x, size.y, 1, format, type, img1.pixels);
}

void Texture::createCubemap(const fspath& folder) {
  // NOTE: Order matters
  constexpr const char* texNames[6] = {
    "right",
    "left",
    "top",
    "bottom",
    "front",
    "back",
  };

  glGenTextures(1, &id);
  bind();
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // ========== Find out the extension of the images ==========  //

  // NOTE: Assuming all files have the same extention (per folder)

  char imgPath[256];
  sprintf(imgPath, "%s/*.*", folder.string().c_str());

  WIN32_FIND_DATA fdFile;
  HANDLE hFind = NULL;
  if((hFind = FindFirstFile(imgPath, &fdFile)) == INVALID_HANDLE_VALUE)
    error(std::format("Path not found: [{}]\n", folder.string().c_str()));

  //Find first file will always return "."
  //    and ".." as the first two directories.
  FindNextFile(hFind, &fdFile);
  FindNextFile(hFind, &fdFile);
  if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    warning(std::format("Warning found directory in the skybox folder: [{}]\n", fdFile.cFileName));

  FindNextFile(hFind, &fdFile);
  sprintf(imgPath, "%s/%s", folder.string().c_str(), fdFile.cFileName);

  std::string extension = fspath(fdFile.cFileName).extension().string();
  FindClose(hFind);

  // ==========================================================  //

  for (int i = 0; i < 6; i++) {
    fspath filePath = std::format("{}/{}{}", folder.string().c_str(), texNames[i], extension.c_str());
    image2D img = image2D(filePath);

    const GLenum& texFormat = internalFormat ? internalFormat : GL_STANDARD_CHANNELS[img.channels - 1];
    const GLenum& imgFormat = format ? format : GL_STANDARD_CHANNELS[img.channels - 1];

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, texFormat, img.width, img.height, 0, imgFormat, type, img.pixels);
  }
}

