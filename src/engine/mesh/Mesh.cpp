#include "Mesh.hpp"

#include <format>

#include "glm/ext/matrix_transform.hpp"
#include "meshes.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

Mesh Mesh::loadObj(const fspath& file, bool printInfo) {
  tinyobj::ObjReaderConfig readerConfig;
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(file.string(), readerConfig)) {
    std::string msg = "ParseFromFile error";
    if (!reader.Error().empty())
      msg = "TinyObjReader: " + reader.Error();

    error(msg);
  }

  if (!reader.Warning().empty())
    warning(std::format("TinyObjReader: {}", reader.Warning()));

  const tinyobj::attrib_t& attrib = reader.GetAttrib();
  const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
  const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();

  std::vector<Vertex> vertices;

  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {
    // Loop over faces(polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

      // Loop over vertices in the face.
      for (size_t v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        size_t idxVert = 3 * size_t(idx.vertex_index);
        Vertex vertex;

        vertex.position.x = attrib.vertices[idxVert + 0];
        vertex.position.y = attrib.vertices[idxVert + 1];
        vertex.position.z = attrib.vertices[idxVert + 2];

        // Check if `normal_index` is zero or positive. negative = no normal data
        if (idx.normal_index >= 0) {
          vertex.normal.x = attrib.normals[3*size_t(idx.normal_index)+0];
          vertex.normal.y = attrib.normals[3*size_t(idx.normal_index)+1];
          vertex.normal.z = attrib.normals[3*size_t(idx.normal_index)+2];
        }

        // Check if `texcoord_index` is zero or positive. negative = no texcoord data
        if (idx.texcoord_index >= 0) {
          vertex.texture.x = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
          vertex.texture.y = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
        }

        // Optional: vertex colors
        vertex.color.x = attrib.colors[3*size_t(idx.vertex_index)+0];
        vertex.color.y = attrib.colors[3*size_t(idx.vertex_index)+1];
        vertex.color.z = attrib.colors[3*size_t(idx.vertex_index)+2];

        vertices.push_back(vertex);
      }
      index_offset += fv;

      // per-face material
      // shapes[s].mesh.material_ids[f];
    }
  }

  // ============ Print info ============ //

  if (printInfo) {
    clrp::clrp_t cfmt{
      .attr = clrp::ATTRIBUTE::BOLD,
      .fg = clrp::FG::CYAN
    };
    std::string cname = clrp::format(std::format("[{}]", file.string()), cfmt);
    std::string infoLoad = std::format("[load]\nvertices: {}\ncolors:   {}\ntextures: {}\nnormals:  {}", attrib.vertices.size() / 3, attrib.colors.size() / 3, attrib.texcoords.size() / 2, attrib.normals.size() / 3);
    std::string infoFinal = std::format("[final]\nvertices: {}\n", vertices.size());
    printf("\n==================== %s ====================\n\n%s\n\n%s\n\n", cname.c_str(), infoLoad.c_str(), infoFinal.c_str());

    std::string end = "============================================";
    for (u32 i = 0; i < file.string().size(); i++)
      end += "=";
    end += "\n";

    puts(end.c_str());
  }

  // ==================================== //

  return Mesh(vertices, GL_TRIANGLES, false);
}

Mesh::Mesh() : clearable(false) {}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, GLenum mode, bool clearable)
  : vertices(vertices),
    indices(indices),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex) * vertices.size())),
    ebo(1, indices.data(), sizeof(GLuint) * indices.size()),
    mode(mode),
    clearable(clearable) {
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

Mesh::Mesh(std::vector<Vertex> vertices, GLenum mode, bool clearable)
  : vertices(vertices),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex) * vertices.size())),
    mode(mode),
    clearable(clearable) {
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

Mesh::~Mesh() {
  if (clearable)
    clear();
}

const mat4& Mesh::getTranslation() const { return translation; }
const mat4& Mesh::getRotation()    const { return rotation;    }
const mat4& Mesh::getScale()       const { return scaleMat;    }

mat4 Mesh::getModel() const {
  return translation * rotation * scaleMat;
}

void Mesh::add(const Texture* texture) {
  if (texCount < MESH_TEXTURE_LIMIT) //
    textures[texCount++] = texture;
  else //
    warning("Trying to add a texture above the limit");
}

void Mesh::clear() {
  if (vao.size) vao.clear();
  if (vbo.size) vbo.clear();
  if (ebo.size) ebo.clear();
}

void Mesh::translate(const vec3& v) { translation = glm::translate(translation, v); }

void Mesh::rotate(const float& angle, const vec3& axis) { rotation = glm::rotate(rotation, angle, axis);}
void Mesh::rotate(const glm::quat& q) { rotation = glm::mat4_cast(q) * rotation; };

void Mesh::scale(const float& s)    { scaleMat = glm::scale(scaleMat, vec3(s)); }
void Mesh::scale(const vec2& s)     { scaleMat = glm::scale(scaleMat, vec3(s, 1.f)); }

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

  if (global::drawWireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  if (indices.size())
    glDrawElements(mode, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
  else
    glDrawArrays(mode, 0, (GLsizei)vertices.size());

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  vao.unbind();
}

void Mesh::drawAxis(const Camera* camera, const Shader& shader) const {
  static Mesh axis = meshes::axis(3.f);
  axis.translation = translation;
  axis.rotation = rotation;
  axis.scaleMat = scaleMat;
  axis.draw(camera, shader);
}

