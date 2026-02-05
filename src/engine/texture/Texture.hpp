#pragma once

#include "TextureDescriptor.hpp"
#include "image2D.hpp"

class Texture {
public:
  static const Texture& getDebug0Tex();

  Texture() = default;

  Texture(const image2D& img, const TextureDescriptor& desc);
  Texture(const ivec2& size, const TextureDescriptor& desc);
  Texture(const fspath& path, const TextureDescriptor& desc);
  Texture(const Texture& other);

  void operator=(const Texture& other);

  void bind(GLuint customUnit) const;
  void bind() const;
  void unbind() const;
  void clear();

  const GLuint& getId() const;
  const GLenum& getTarget() const;
  const GLuint& getUnit() const;
  const std::string& getUniformName() const;
  // const uvec3& getSize() const;

  void setUnit(GLuint unit);
  void setUniformName(const std::string& name);

private:
  static Texture debug0Tex;

  TextureDescriptor desc{};
  GLuint id = 0;

private:
  void create2D(const image2D& img);
  void create2DArray(const fspath& folder);
  void createCubemap(const fspath& folder);
};

