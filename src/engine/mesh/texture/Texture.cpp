#include "Texture.hpp"

#include <format>
#include <windows.h>

constexpr u32 GLchannels[4] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};

Texture::Texture() {}

Texture::Texture(const image2D& img, GLenum type, std::string uniform, GLuint unit, u8 prefChannels)
  : unit(unit),
    uniformName(uniform) {
  switch (type) {
    case GL_TEXTURE_2D:
      create2D(img, prefChannels);
    default:
      error(std::format("Unhandled texture creation type: [{}]", type));
  }

  unbind();
}

Texture::Texture(const fspath& path, GLenum type, std::string uniform, GLuint unit, u8 prefChannels)
  : unit(unit),
    uniformName(uniform) {
  glGenTextures(1, &id);

  switch (type) {
    case GL_TEXTURE_2D: {
      create2D(image2D(path), prefChannels);
      break;
    }
    case GL_TEXTURE_CUBE_MAP: {
      glTexParameteri(glType, GL_TEXTURE_WRAP_R, GL_REPEAT);
      glType = GL_TEXTURE_CUBE_MAP;
      image2D img;

      WIN32_FIND_DATA fdFile;
      HANDLE hFind = NULL;
      if ((hFind = FindFirstFile((path / "*.*").string().c_str(), &fdFile)) == INVALID_HANDLE_VALUE)
        error(std::format("Path not found: [{}]", path.string().c_str()));

      // [FindNextFile] will always return "." on first call and ".." as the first two directories.
      FindNextFile(hFind, &fdFile);
      FindNextFile(hFind, &fdFile);
      if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        error(std::format("Found directory in the skybox folder: [{}]", fdFile.cFileName));

      FindNextFile(hFind, &fdFile);
      std::string extension = fspath(fdFile.cFileName).extension().string();
      FindClose(hFind);

      // NOTE: All files should have a same extension
      for (u8 i = 0; i < 6; i++) {
        constexpr char const* names[6] = {"right", "left", "top", "bottom", "back", "front"};
        img.load(fspath(names[i]) / extension);
        const GLint& imgFormat = GLchannels[img.channels - 1];
        glTexImage2D(
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, imgFormat, img.width, img.height, 0, imgFormat, GL_UNSIGNED_BYTE,
          img.pixels
        );
      }

      break;
    }
    default:
      printf("Unhandled texture type (%d)\n", type);
      printf("Path: %s\n", path.string().c_str());
      exit(EXIT_FAILURE);
  }

  unbind();
}

void Texture::bind() const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(glType, id);
}

void Texture::unbind() const {
  glBindTexture(glType, 0); //
}

void Texture::clear() {
  glDeleteTextures(1, &id); //
}

const GLenum& Texture::getType() const { return glType; }
const GLuint& Texture::getUnit() const { return unit; }
const std::string& Texture::getUniformName() const { return uniformName; }
const uvec2& Texture::getSize() const { return size; }

void Texture::create2D(const image2D& img, u8 prefChannels) {
  printf("Creating [%s]\n", img.name.c_str());
  size = {img.width, img.height};
  glType = GL_TEXTURE_2D;
  glGenTextures(1, &id);
  bind();
  glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(glType, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(glType, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // NOTE: Not all image format can be internal format
  u8 idx = prefChannels ? prefChannels - 1 : img.channels - 1;
  u32 formatInternal = GLchannels[idx];
  u32 formatImg = GLchannels[img.channels - 1];
  glTexImage2D(glType, 0, formatInternal, img.width, img.height, 0, formatImg, GL_UNSIGNED_BYTE, img.pixels);
}

