#pragma once

// Vertex Buffer Object
class VBO {
public:
  VBO();
  VBO(GLsizei size, const void* data, GLsizeiptr dataSize);

  void bind() const;
  void clear() const;
  void unbind() const;

private:
  GLuint id;
  GLsizei size;
};

