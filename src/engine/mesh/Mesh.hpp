#pragma once

#include <vector>

#include "../Camera.hpp"
#include "../Shader.hpp"
#include "EBO.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "Vertex.hpp"
#include "texture/Texture.hpp"

#define MESH_VERTEX_ATTRIBUTES 11 // 3 (position) + 3 (color) + 2 (texture) + 3 (normal)
#define MESH_TEXTURE_LIMIT 10

class Mesh {
public:
  Mesh();
  Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, GLenum mode = GL_TRIANGLES);
  Mesh(std::vector<Vertex> vertices, GLenum mode);

  const mat4& getTranslation() const;
  const mat4& getRotation()    const;
  const mat4& getScale()       const;

  void add(const Texture* texture);
  void translate(vec3 v);
  void rotate(vec3 axis, float angle);
  void scale(float s);
  void scale(vec2 s);

  void draw(const Camera* camera, const Shader& shader) const;
  void drawAxis(const Camera* camera, const Shader& shader) const;

protected:
  mat4 translation = identity<mat4>();
  mat4 rotation    = identity<mat4>();
  mat4 scaleMat    = identity<mat4>();

private:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  GLenum mode;

  const Texture* textures[MESH_TEXTURE_LIMIT];
  u8 texCount = 0;

  VAO vao;
  VBO vbo;
  EBO ebo;
};
