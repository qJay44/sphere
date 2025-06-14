#pragma once

#include "MeshBase.hpp"

#include "EBO.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "vertex.hpp"

template<typename T>
class Mesh : public MeshBase {
public:
  Mesh() {};

  Mesh(const std::vector<Vertex4>&  vertices, const std::vector<GLuint>& indices, GLenum mode = GL_TRIANGLES, bool clearable = true);
  Mesh(const std::vector<VertexPT>& vertices, const std::vector<GLuint>& indices, GLenum mode = GL_TRIANGLES, bool clearable = true);
  Mesh(const std::vector<Vertex1>&  vertices, const std::vector<GLuint>& indices, GLenum mode = GL_TRIANGLES, bool clearable = true);
  Mesh(const std::vector<Vertex4>& vertices, GLenum mode, bool clearable = true);
  Mesh(const std::vector<Vertex1>& vertices, GLenum mode, bool clearable = true);

  static Mesh<Vertex4> loadObj(const fspath& file, bool printInfo = false);

  ~Mesh() {
    if (clearable)
      clear();
  }

  u32 getVerticesSize() const { return vertices.size(); }

  void draw(const Camera* camera, const Shader& shader) const {
    vao.bind();

    mat4 model = translation * rotation * scaleMat;

    shader.setUniform3f("u_camPos", camera->getPosition());
    shader.setUniformMatrix4f("u_cam", camera->getMatrix());
    shader.setUniformMatrix4f("u_model", model);

    if (global::drawWireframe)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (indices.size())
      glDrawElements(mode, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    else
      glDrawArrays(mode, 0, (GLsizei)vertices.size());

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    vao.unbind();
  }

  void clear() {
    if (vao.size) vao.clear();
    if (vbo.size) vbo.clear();
    if (ebo.size) ebo.clear();
  }

private:
  std::vector<T> vertices;

  VAO vao;
  VBO vbo;
  EBO ebo;
};

