#pragma once

// Element Buffer Object
struct EBO {
  GLuint id;
  GLsizei size = 0;

  EBO() {}

  EBO(GLsizei size, const void* data, GLsizeiptr dataSize) : size(size) {
    glGenBuffers(size, &id);
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
  }

  void bind()   const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); }
  void unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
  void clear()        { glDeleteBuffers(size, &id); size = 0; }
};

