#pragma once

#include <vector>

#include "Vertex.hpp"
#include "Transformable.hpp"
#include "VAO.hpp"
#include "BufferObject.hpp"
#include "../Camera.hpp"
#include "../Shader.hpp"

class Mesh : public Transformable {
public:
  Mesh(const std::vector<Vertex4>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool clearable);
  Mesh(const std::vector<VertexPT>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool clearable);
  Mesh(const std::vector<VertexPC>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool clearable);
  ~Mesh();

  static Mesh loadObj(const fspath& file, bool printInfo = false);
  static void screenDraw(const Camera* camera, Shader& shader);

  void clear();
  void draw(const Camera* camera, Shader& shader, bool forceNoWireframe = false) const;

private:
  GLsizei count = 0;
  GLenum mode;
  bool autoClear;
  VAO vao;
  BufferObject vbo{GL_ARRAY_BUFFER};
  BufferObject ebo{GL_ELEMENT_ARRAY_BUFFER};

private:
  static void setCamUniforms(const Camera* c, Shader& s);
};


