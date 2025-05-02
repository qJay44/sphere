#include "Mesh.hpp"

#include "glm/ext/matrix_transform.hpp"
#include "meshes.hpp"

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, GLenum mode)
  : vertices(vertices),
    indices(indices),
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

const mat4& Mesh::getTranslation() const { return translation; }
const mat4& Mesh::getRotation()    const { return rotation;    }
const mat4& Mesh::getScale()       const { return scaleMat;    }

void Mesh::add(const Texture* texture) {
  if (texCount < MESH_TEXTURE_LIMIT) //
    textures[texCount++] = texture;
  else //
    warning("Trying to add a texture above the limit");
}

void Mesh::scale(const float& s)    { scaleMat = glm::scale(scaleMat, vec3(s)); }
void Mesh::scale(const vec2& s)     { scaleMat = glm::scale(scaleMat, vec3(s, 1.f)); }
void Mesh::translate(const vec3& v) { translation = glm::translate(translation, v); }

void Mesh::rotate(const float& angle, const vec3& axis) { rotation = glm::rotate(rotation, angle, axis);}
void Mesh::rotate(const glm::quat& q) { rotation *= glm::mat4_cast(q); };

void Mesh::draw(const Camera* camera, const Shader& shader) const {
  vao.bind();

  for (u8 i = 0; i < texCount; i++) {
    const Texture* tex = textures[i];
    shader.setUniformTexture(tex->getUniformName(), tex->getUnit());
    tex->bind();
  }

  mat4 model = translation * rotation * scaleMat;

  shader.setUniform3f("camPos", camera->getPosition());
  shader.setUniformMatrix4f("cam", camera->getMatrix());
  shader.setUniformMatrix4f("model", model);

  if (indices.size()) glDrawElements(mode, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
  else glDrawArrays(mode, 0, (GLsizei)vertices.size());

  vao.unbind();
}

void Mesh::drawAxis(const Camera* camera, const Shader& shader) const {
  static Mesh axis = meshes::axis(3.f);
  axis.translation = translation;
  axis.rotation = rotation;
  axis.scaleMat = scaleMat;
  axis.draw(camera, shader);
}

