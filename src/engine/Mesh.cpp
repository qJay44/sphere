#include "Mesh.hpp"

#include "glm/ext/matrix_transform.hpp"

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices)
  : vertices(vertices),
    indices(indices),
    mat(identity<mat4>()),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex) * vertices.size())),
    ebo(1, indices.data(), sizeof(GLuint) * indices.size()) {

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

Mesh::Mesh(std::vector<Vertex> vertices)
  : vertices(vertices),
    mat(identity<mat4>()),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex) * vertices.size())) {

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

void Mesh::draw(const Camera& camera, const Shader& shader) const {
  vao.bind();

  shader.setUniform3f("camPos", camera.getPosition());
  shader.setUniformMatrix4f("cam", camera.getMatrix());
  shader.setUniformMatrix4f("model", mat);

  if (indices.size()) glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
  else glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
  vao.unbind();
}
