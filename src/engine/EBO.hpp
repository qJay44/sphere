#pragma once

// Element Buffer Object
class EBO {
public:
  EBO();
  EBO(GLsizei size, const void* data, GLsizeiptr dataSize);

  void bind() const;
  void clear() const;
  void unbind() const;

private:
  GLuint id;
  GLsizei size;
};

