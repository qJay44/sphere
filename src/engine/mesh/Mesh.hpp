#pragma once

#include <functional>
#include <vector>

#include "Vertex.hpp"
#include "Transformable.hpp"
#include "VAO.hpp"
#include "BufferObject.hpp"
#include "../Camera.hpp"
#include "../Shader.hpp"

class Mesh : public Transformable {
public:
  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;

  Mesh(Mesh &&other);

  Mesh(const std::vector<Vertex4>&  vertices, const std::vector<GLuint>& indices, GLenum mode);
  Mesh(const std::vector<VertexPT>& vertices, const std::vector<GLuint>& indices, GLenum mode);
  Mesh(const std::vector<VertexPC>& vertices, const std::vector<GLuint>& indices, GLenum mode);

  Mesh(const std::vector<Vertex4>& vertices, GLenum mode, GLenum usage);

  ~Mesh();

  static Mesh loadObj(const fspath& file, bool printInfo = false);
  static void screenDraw(const Camera* camera, Shader& shader);

  template<typename T>
  void updateData(const std::vector<T>& vertices, GLenum usage) {
    vbo.allocate(vertices, usage);
    count = vertices.size();
  }

  void draw(const Camera* camera, Shader& shader, bool forceNoWireframe = false) const;
  void clear();

private:
  GLsizei count = 0;
  GLenum mode = 0;
  VAO vao;
  BufferObject vbo{GL_ARRAY_BUFFER};
  BufferObject ebo{GL_ELEMENT_ARRAY_BUFFER};

  std::function<void(GLenum mode, GLsizei count)> drawFunc;

private:
  static void setCamUniforms(const Camera* c, Shader& s);

  static void drawElements(GLenum mode, GLsizei count);
  static void drawArrays(GLenum mode, GLsizei count);
};


