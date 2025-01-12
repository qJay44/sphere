#pragma once

#include <vector>

#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"

#define MESH_VERTEX_ATTRIBUTES 11 // 3 (position) + 3 (color) + 2 (texture) + 3 (normal)

class Mesh {
public:
  Mesh();
  Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
  Mesh(std::vector<Vertex> vertices);

  void draw(const Camera& camera, const Shader& shader) const;

private:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  mat4 mat;

  VAO vao;
  VBO vbo;
  EBO ebo;
};
