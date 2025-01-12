#include "VBO.hpp"

VBO::VBO() {}

VBO::VBO(GLsizei size, const void* data, GLsizeiptr dataSize) : size(size) {
  glGenBuffers(size, &id);
  bind();
  glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
}

void VBO::bind() const {
  glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VBO::clear() const {
  glDeleteBuffers(size, &id);
}

void VBO::unbind() const {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

