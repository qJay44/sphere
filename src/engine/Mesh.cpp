#include "Mesh.hpp"
#include "glm/ext/matrix_transform.hpp"

Mesh::Mesh(std::vector<float> vertices, std::vector<GLuint> indices)
  : vertices(vertices),
    indices(indices),
    mat(identity<mat4>()),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(float) * vertices.size())),
    ebo(1, indices.data(), sizeof(GLuint) * indices.size())
{
  vao.bind();
  vbo.bind();
  ebo.bind();

  size_t typeSize = sizeof(GLfloat);
  size_t stride = MESH_VERTEX_ATTRIBUTES * typeSize;

  vao.linkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vao.linkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize));
  vao.linkAttrib(2, 2, GL_FLOAT, stride, (void*)(6 * typeSize));
  vao.linkAttrib(3, 3, GL_FLOAT, stride, (void*)(8 * typeSize));

  vao.unbind();
  vbo.unbind();
  ebo.unbind();
}

void Mesh::draw(const Camera& camera, const Shader& shader) const {
  shader.activate();
  vao.bind();

  shader.setUniform3f("camPosUni", camera.getPosition());
  shader.setUniformMatrix4f("cam", camera.getMatrix());
  shader.setUniformMatrix4f("model", mat);

  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  vao.unbind();
}
