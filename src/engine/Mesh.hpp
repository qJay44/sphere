#pragma once

#include <vector>

#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "camera.hpp"
#include "Shader.hpp"

#define MESH_VERTEX_ATTRIBUTES 11 // 3 (position) + 3 (color) + 2 (texture) + 3 (normal)

class Mesh {
public:
  Mesh(std::vector<float> vertices, std::vector<GLuint> indices);

  void draw(const Camera& camera, const Shader& shader) const;

private:
  std::vector<float> vertices;
  std::vector<GLuint> indices;
  mat4 mat;

  VAO vao;
  VBO vbo;
  EBO ebo;
};
