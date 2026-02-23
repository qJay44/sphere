#pragma once

struct TextureDescriptorVirtual {
  // Will be overwritten in the constructor
  std::string name{};
  fspath filePath;
  uvec2 unitsPhyInd;

  int tileSlots;
  ivec2 tileSize;
  ivec2 virtualDims;

  // Only affects physical texture
  GLenum internalFormat = GL_RGB8;
  GLenum format         = GL_RGB;
  GLenum minFilter      = GL_LINEAR;
  GLenum magFilter      = GL_LINEAR;
};

