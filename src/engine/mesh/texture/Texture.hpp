#pragma once

#include "image2D.hpp"

class Texture {
public:
  Texture();
  Texture(const image2D& img0, const image2D& img1, const GLuint& unit = 0);
  Texture(const image2D& img, const GLenum& type, const std::string& uniform, const GLuint& unit = 0, const u8& prefChannels = 0);
  Texture(const fspath& path, const GLenum& type, const std::string& uniform, const GLuint& unit = 0, const u8& prefChannels = 0);

  void bind() const;
  void unbind() const;
  void clear();

  const GLenum& getType() const;
  const GLuint& getUnit() const;
  const std::string& getUniformName() const;
  const uvec3& getSize() const;

private:
  GLuint unit;
  std::string uniformName;

  GLuint id;
  GLenum glType;
  uvec3 size;

private:
  void create2D(const image2D& img, u8 prefChannels);
  void create2DArray(const image2D& img0, const image2D& img1);
};

