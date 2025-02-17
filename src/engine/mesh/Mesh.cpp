#include "Mesh.hpp"

#include <format>

#include "glm/ext/matrix_transform.hpp"

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, GLenum mode)
  : vertices(vertices),
    indices(indices),
    mat(identity<mat4>()),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex) * vertices.size())),
    ebo(1, indices.data(), sizeof(GLuint) * indices.size()),
    mode(mode) {
  vao.bind();
  vbo.bind();
  ebo.bind();

  size_t typeSize = sizeof(GLfloat);
  GLsizei stride = static_cast<GLsizei>(MESH_VERTEX_ATTRIBUTES * typeSize);

  vao.linkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vao.linkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize));
  vao.linkAttrib(2, 2, GL_FLOAT, stride, (void*)(6 * typeSize));
  vao.linkAttrib(3, 3, GL_FLOAT, stride, (void*)(8 * typeSize));

  vao.unbind();
  vbo.unbind();
  ebo.unbind();
}

Mesh::Mesh(std::vector<Vertex> vertices, GLenum mode)
  : vertices(vertices),
    mat(identity<mat4>()),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex) * vertices.size())),
    mode(mode) {
  vao.bind();
  vbo.bind();

  size_t typeSize = sizeof(GLfloat);
  GLsizei stride = static_cast<GLsizei>(MESH_VERTEX_ATTRIBUTES * typeSize);

  vao.linkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vao.linkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize));
  vao.linkAttrib(2, 2, GL_FLOAT, stride, (void*)(6 * typeSize));
  vao.linkAttrib(3, 3, GL_FLOAT, stride, (void*)(8 * typeSize));

  vao.unbind();
  vbo.unbind();
}

void Mesh::add(const Texture* texture) {
  if (texCount < MESH_TEXTURE_LIMIT) //
    textures[texCount++] = texture;
  else //
    warning("Trying to add a texture above the limit");
}

void Mesh::scale(float s) { mat = glm::scale(mat, vec3(s)); }
void Mesh::scale(vec2 s) { mat = glm::scale(mat, vec3(s, 1.f)); }
void Mesh::translate(vec3 v) { mat = glm::translate(mat, v); }

void Mesh::draw(const Camera& camera, const Shader& shader) const {
  vao.bind();

  for (u8 i = 0; i < texCount; i++) {
    const Texture* tex = textures[i];
    shader.setUniformTexture(tex->getUniformName(), tex->getUnit());
    tex->bind();
  }

  shader.setUniform3f("camPos", camera.getPosition());
  shader.setUniformMatrix4f("cam", camera.getMatrix());
  shader.setUniformMatrix4f("model", mat);

  if (indices.size()) glDrawElements(mode, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
  else glDrawArrays(mode, 0, (GLsizei)vertices.size());

  vao.unbind();
}
