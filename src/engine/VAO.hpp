#pragma once

// Vertex Array Object
class VAO {
public:
  VAO();
  VAO(GLsizei size);

  void linkAttrib(GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset) const;
  void bind() const;
  void clear() const;
  void unbind() const;

private:
  GLuint id;
  GLsizei size;
};

