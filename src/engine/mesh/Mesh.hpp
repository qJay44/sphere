#pragma once

#include <vector>

#include "glm/gtc/quaternion.hpp"

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
  static Mesh loadObj(const fspath& file, bool printInfo = false);

  Mesh();
  Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, GLenum mode = GL_TRIANGLES, bool clearable = true);
  Mesh(std::vector<Vertex> vertices, GLenum mode, bool clearable = true);

  ~Mesh();

  const mat4& getTranslation() const;
  const mat4& getRotation()    const;
  const mat4& getScale()       const;
  mat4 getModel() const;

  void add(const Texture* texture);
  void clear();

  void translate(const vec3& v);
  void rotate(const float& angle, const vec3& axis);
  void rotate(const glm::quat& q);
  void scale(const float& s);
  void scale(const vec2& s);

  void draw(const Camera* camera, const Shader& shader) const;
  void drawAxis(const Camera* camera, const Shader& shader) const;

protected:
  mat4 translation = mat4(1.f);
  mat4 rotation    = mat4(1.f);
  mat4 scaleMat    = mat4(1.f);

  bool clearable;

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

