#include "Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Texture::Texture() {}

Texture::Texture(const fspath& path, TextureType type)
  : type(type),            //
    glType(GL_TEXTURE_2D), //
    name(path.filename()) {

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  int w, h, colorChannels;
  stbi_set_flip_vertically_on_load(true);
  byte* bytes = stbi_load(path.string().c_str(), &w, &h, &colorChannels, 0);

  switch (type) {
    default:
      switch (colorChannels) {
        case 1:
          glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
          break;
        case 3:
          glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
          break;
        case 4:
          glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
          break;
        default:
          printf("Unhandled texture image color channels type (%d)\n", colorChannels);
          printf("Path: %s\n", path.string().c_str());
          exit(EXIT_FAILURE);
      }
  }

  glGenerateMipmap(GL_TEXTURE_2D);
  unbind();
  stbi_image_free(bytes);
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

const TextureType& Texture::getType() const { return type; }
const GLuint& Texture::getUnit() const { return unit; }
