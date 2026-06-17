#include "Texture.hpp"

Texture::Texture(Texture&& other)
  : desc(other.desc),
    id(other.id)
{
  other.id = 0;
}

Texture& Texture::operator=(Texture&& other) {
  if (this != &other) {
    clear();
    desc = other.desc;
    id = other.id;
    other.id = 0;
  }

  return *this;
}

Texture::~Texture() {
  clear();
}

void Texture::bind(GLuint unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(desc.target, id);
}

void Texture::unbind() const {
  glBindTexture(desc.target, 0);
}

void Texture::clear() {
  if (id) glDeleteTextures(1, &id);
  id = 0;
}

const GLuint& Texture::getId() const { return id; }
const GLenum& Texture::getTarget() const { return desc.target; }

ivec2 Texture::getSize(GLint mipLevel) const {
  ivec2 res;

  bind(0);
  glGetTexLevelParameteriv(desc.target, mipLevel, GL_TEXTURE_WIDTH, &res.x);
  glGetTexLevelParameteriv(desc.target, mipLevel, GL_TEXTURE_HEIGHT, &res.y);
  unbind();

  return res;
}

Texture::Texture(const TextureDescriptor& desc) : desc(desc) {}

