#include "EBO.hpp"

EBO::EBO(GLsizei size, const void* data, GLsizeiptr dataSize) : size(size) {
  glGenBuffers(size, &id);
  bind();
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
}

void EBO::bind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void EBO::clear() const {
  glDeleteBuffers(size, &id);
}

void EBO::unbind() const {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

