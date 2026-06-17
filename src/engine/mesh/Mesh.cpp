#include "Mesh.hpp"

#include "global.hpp"
#include "utils/status.hpp"
#include "utils/clrp.hpp"
#include "../gui/gui.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// A custom hasher for the tinyobj index struct
struct IndexHasher {
  size_t operator() (const struct tinyobj::index_t& i) const {
    size_t h = 0;
    // Standard hash combine logic
    auto hash_combine = [](size_t& seed, int v) {
      seed ^= std::hash<int>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    };

    hash_combine(h, i.vertex_index);
    hash_combine(h, i.normal_index);
    hash_combine(h, i.texcoord_index);
    return h;
  }
};

// Equality check for the map
struct IndexEqual {
  bool operator() (const tinyobj::index_t& a, const tinyobj::index_t& b) const {
    return a.vertex_index == b.vertex_index &&
           a.normal_index == b.normal_index &&
           a.texcoord_index == b.texcoord_index;
  }
};

Mesh Mesh::loadObj(const fspath& file, bool printInfo) {
  tinyobj::ObjReaderConfig readerConfig;
  tinyobj::ObjReader reader;

  status::start("Loading", file.string());

  if (!reader.ParseFromFile(file.string(), readerConfig)) {
    std::string msg = "ParseFromFile error";
    if (!reader.Error().empty())
      msg = "TinyObjReader: " + reader.Error();

    status::end(false);
    error(msg);
  }

  if (!reader.Warning().empty())
    warning(std::format("TinyObjReader: {}", reader.Warning()));

  const tinyobj::attrib_t& attrib = reader.GetAttrib();
  const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
  // const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();

  std::vector<Vertex4> vertices;
  std::vector<GLuint> indices;
  std::unordered_map<tinyobj::index_t, uint32_t, IndexHasher, IndexEqual> uniqueVertices;

  for (const auto& shape : shapes) {
    for (const auto& idx : shape.mesh.indices) {
      auto [it, inserted] = uniqueVertices.emplace(idx, vertices.size());

      if (inserted) {
        Vertex4 vertex;

        vertex.position = {
          attrib.vertices[3 * idx.vertex_index + 0],
          attrib.vertices[3 * idx.vertex_index + 1],
          attrib.vertices[3 * idx.vertex_index + 2]
        };

        // Check if `normal_index` is zero or positive. negative = no normal data
        if (idx.normal_index >= 0) {
          vertex.normal = {
            attrib.normals[3 * idx.normal_index + 0],
            attrib.normals[3 * idx.normal_index + 1],
            attrib.normals[3 * idx.normal_index + 2]
          };
        }

        // Check if `texcoord_index` is zero or positive. negative = no texcoord data
        if (idx.texcoord_index >= 0) {
          vertex.texture = {
            attrib.texcoords[2 * idx.texcoord_index + 0],
            attrib.texcoords[2 * idx.texcoord_index + 1]
          };
        }

        // Optional: vertex colors
        vertex.color = {
          attrib.colors[3 * idx.vertex_index + 0],
          attrib.colors[3 * idx.vertex_index + 1],
          attrib.colors[3 * idx.vertex_index + 2]
        };

        vertices.push_back(vertex);
      }

      indices.push_back(it->second);
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
    std::string infoFinal = std::format("[final]\nvertices: {}, indices: {}\n", vertices.size(), indices.size());
    printf("\n==================== %s ====================\n\n%s\n\n%s\n\n", cname.c_str(), infoLoad.c_str(), infoFinal.c_str());

    std::string end = "============================================";
    for (u32 i = 0; i < file.string().size(); i++)
      end += "=";
    end += "\n";

    puts(end.c_str());
  }

  // ==================================== //

  status::end(true);

  return Mesh(vertices, indices, GL_TRIANGLES);
}

Mesh::Mesh(const std::vector<Vertex4>&  vertices, const std::vector<GLuint>& indices, GLenum mode, GLenum usage) : Mesh(std::span(vertices), indices, mode, usage) {}
Mesh::Mesh(const std::vector<VertexPT>& vertices, const std::vector<GLuint>& indices, GLenum mode, GLenum usage) : Mesh(std::span(vertices), indices, mode, usage) {}
Mesh::Mesh(const std::vector<VertexPC>& vertices, const std::vector<GLuint>& indices, GLenum mode, GLenum usage) : Mesh(std::span(vertices), indices, mode, usage) {}

Mesh::Mesh(const std::vector<Vertex4>&  vertices, GLenum mode, GLenum usage) : Mesh(std::span(vertices), {}, mode, usage) {}
Mesh::Mesh(const std::vector<VertexPT>& vertices, GLenum mode, GLenum usage) : Mesh(std::span(vertices), {}, mode, usage) {}
Mesh::Mesh(const std::vector<VertexPC>& vertices, GLenum mode, GLenum usage) : Mesh(std::span(vertices), {}, mode, usage) {}
Mesh::Mesh(const std::vector<VertexP>&  vertices, GLenum mode, GLenum usage) : Mesh(std::span(vertices), {}, mode, usage) {}

void Mesh::drawScreen(const Camera* camera, Shader& shader) {
  static const VAO& vao = VAO::getEmpty();
  camera->setUniforms(shader);
  setGlobalUniforms(shader);
  setExtraUniforms(shader);

  vao.bind();
  shader.use();
  glDrawArrays(GL_TRIANGLES, 0, 6);
  vao.unbind();
}

void Mesh::drawDirectionLine(const Camera* camera, Shader& shader, const vec3& p, const vec3& d, const vec3& color) {
  static const VAO& vao = VAO::getEmpty();
  vao.bind();

  camera->setUniforms(shader);
  setGlobalUniforms(shader);
  setExtraUniforms(shader);

  shader.use();
  shader.setUniform3f("u_pos", p);
  shader.setUniform3f("u_dir", d);
  shader.setUniform3f("u_color", color);

  glDrawArrays(GL_LINES, 0, 2);
  vao.unbind();
}

void Mesh::draw(const Camera* camera, Shader& shader) const {
  assert(count);
  assert(mode);

  vao.bind();

  camera->setUniforms(shader);
  setGlobalUniforms(shader);
  setExtraUniforms(shader);

  shader.setUniformMatrix4f("u_model", getModel());

  shader.use();
  drawFunc(mode, count);

  vao.unbind();
}

void Mesh::setGlobalUniforms(Shader& s) {
  s.setUniform1f("u_globalTime", global::time);
  s.setUniform1f("u_globalGamma", global::gamma);
}

void Mesh::setExtraUniforms(Shader& s) {
  s.setUniform1f("u_1f0", gui::_slider1f0);
  s.setUniform1f("u_1f1", gui::_slider1f1);
  s.setUniform1f("u_1f2", gui::_slider1f2);
  s.setUniform2f("u_2f0", gui::_slider2f0);
  s.setUniform2f("u_2f1", gui::_slider2f1);
  s.setUniform2f("u_2f2", gui::_slider2f2);
  s.setUniform3f("u_3f0", gui::_slider3f0);
  s.setUniform3f("u_3f1", gui::_slider3f1);
  s.setUniform3f("u_3f2", gui::_slider3f2);
}

void Mesh::drawElements(GLenum mode, GLsizei count) {
  glDrawElements(mode, count, GL_UNSIGNED_INT, 0);
}

void Mesh::drawArrays(GLenum mode, GLsizei count) {
  glDrawArrays(mode, 0, count);
}

