#include "VAO.hpp"

VAO::VAO(GLsizei size) : size(size) {
  glGenVertexArrays(size, &id);
}

void VAO::linkAttrib(GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) const {
  glEnableVertexAttribArray(layout);
  glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
}

void VAO::bind() const {
  glBindVertexArray(id);
}

void VAO::clear() const {
  glDeleteVertexArrays(size, &id);
}

void VAO::unbind() const {
  glBindVertexArray(0);
}

