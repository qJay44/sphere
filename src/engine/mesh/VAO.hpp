#pragma once

// Vertex Array Object
struct VAO {
  GLuint id;
  GLsizei size = 0;

  VAO() {}

  VAO(GLsizei size) : size(size) {
    glGenVertexArrays(size, &id);
  }

  void bind()   const { glBindVertexArray(id); }
  void unbind() const { glBindVertexArray(0); }
  void clear()        { glDeleteVertexArrays(size, &id); size = 0; }

  void linkAttrib(GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset) const {
    glEnableVertexAttribArray(layout);
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
  }
};

