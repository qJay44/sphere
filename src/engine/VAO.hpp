#pragma once

// Vertex Array Object
class VAO {
public:
  VAO(GLsizei size);

  void linkAttrib(GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) const;
  void bind() const;
  void clear() const;
  void unbind() const;

private:
  GLuint id;
  GLsizei size;
};

