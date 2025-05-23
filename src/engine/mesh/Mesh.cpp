#include "Mesh.hpp"

#include "shapefile.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

template<>
Mesh<Vertex4>::Mesh(const std::vector<Vertex4>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool clearable)
  : MeshBase(indices, mode, clearable),
    vertices(vertices),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex4) * vertices.size())),
    ebo(1, indices.data(), sizeof(GLuint) * indices.size())
{
  this->vertices.resize(vertices.size());
  this->vertices.reserve(vertices.size());

  vao.bind();
  vbo.bind();
  ebo.bind();

  size_t typeSize = sizeof(float);
  GLsizei stride = static_cast<GLsizei>(MESH_VERTEX_ATTRIBUTES * typeSize);

  vao.linkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vao.linkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize));
  vao.linkAttrib(2, 2, GL_FLOAT, stride, (void*)(6 * typeSize));
  vao.linkAttrib(3, 3, GL_FLOAT, stride, (void*)(8 * typeSize));

  vao.unbind();
  vbo.unbind();
  ebo.unbind();
}

template<>
Mesh<Vertex1>::Mesh(const std::vector<Vertex1>& vertices, const std::vector<GLuint>& indices, GLenum mode, bool clearable)
  : MeshBase(indices, mode, clearable),
    vertices(vertices),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex1) * vertices.size())),
    ebo(1, indices.data(), sizeof(GLuint) * indices.size())
{
  this->vertices.resize(vertices.size());
  this->vertices.reserve(vertices.size());

  vao.bind();
  vbo.bind();
  ebo.bind();

  vao.linkAttrib(0, 3, GL_FLOAT, 3 * sizeof(float), (void*)(0));

  vao.unbind();
  vbo.unbind();
  ebo.unbind();
}

template<>
Mesh<Vertex4>::Mesh(const std::vector<Vertex4>& vertices, GLenum mode, bool clearable)
  : MeshBase(std::vector<GLuint>(), mode, clearable),
    vertices(vertices),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex4) * vertices.size()))
{
  this->vertices.resize(vertices.size());
  this->vertices.reserve(vertices.size());
  vao.bind();
  vbo.bind();

  size_t typeSize = sizeof(float);
  GLsizei stride = static_cast<GLsizei>(MESH_VERTEX_ATTRIBUTES * typeSize);

  vao.linkAttrib(0, 3, GL_FLOAT, stride, (void*)(0 * typeSize));
  vao.linkAttrib(1, 3, GL_FLOAT, stride, (void*)(3 * typeSize));
  vao.linkAttrib(2, 2, GL_FLOAT, stride, (void*)(6 * typeSize));
  vao.linkAttrib(3, 3, GL_FLOAT, stride, (void*)(8 * typeSize));

  vao.unbind();
  vbo.unbind();
}

template<>
Mesh<Vertex1>::Mesh(const std::vector<Vertex1>& vertices, GLenum mode, bool clearable)
  : MeshBase(std::vector<GLuint>(), mode, clearable),
    vertices(vertices),
    vao(VAO(1)),
    vbo(VBO(1, vertices.data(), sizeof(Vertex1) * vertices.size()))
{
  this->vertices.resize(vertices.size());
  this->vertices.reserve(vertices.size());
  vao.bind();
  vbo.bind();

  vao.linkAttrib(0, 3, GL_FLOAT, 3 * sizeof(float), (void*)(0));

  vao.unbind();
  vbo.unbind();
}

template<>
Mesh<Vertex4> Mesh<Vertex4>::loadObj(const fspath& file, bool printInfo) {
  tinyobj::ObjReaderConfig readerConfig;
  tinyobj::ObjReader reader;
  status::start("Loading obj", file.string());

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
  const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();

  std::vector<Vertex4> vertices;

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
        Vertex4 vertex;

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

  status::end(true);

  return Mesh<Vertex4>(vertices, GL_TRIANGLES, false);
}

template<>
Mesh<Vertex1> Mesh<Vertex1>::loadShapefile(const fspath& folder) {
  constexpr std::endian elittle = std::endian::little;
  constexpr std::endian ebig = std::endian::big;

  using namespace shapefile;
  ShapefileReader shp(folder);
  shapefile_t type = shp.getType();

  std::vector<Vertex1> vertices;
  std::vector<GLuint> indices;
  GLuint idx = 0;

  switch (type) {
    case shapefile_t::Polygon: {
      s32 recordsSize = shp.getRecordsSizeInBytes();
      s32 recordOffset = 0;

      while (recordOffset < recordsSize) {
        const char* recordHeader = shp.getFirstRecordPtr() + recordOffset;

        s32 recordNumber = shp.toInt32(recordHeader + 0, ebig);
        s32 contentLength = shp.toInt32(recordHeader + 4, ebig);

        const char* recordContent = recordHeader + 8;
        shapefile_t rType = (shapefile_t)shp.toInt32(recordContent, elittle);

        if (type != rType)
          error("[Mesh] Shapefile record type differs from the type in the main file header");

        // double Xmin = shp.toDouble(recordContent + 4, elittle);
        // double Ymin = shp.toDouble(recordContent + 12, elittle);
        // double Xmax = shp.toDouble(recordContent + 20, elittle);
        // double Ymax = shp.toDouble(recordContent + 28, elittle);
        s32 numParts = shp.toInt32(recordContent + 36, elittle);
        s32 numPoints = shp.toInt32(recordContent + 40, elittle);
        s32 parts[numParts];
        parts[0] = 0;

        const char* pointsPtr = recordContent + 44 + numParts * 4;

        for (u32 i = 0; i < numParts; i++) {
          s32& currPart = parts[i];

          u32 currPartPoints;
          if (i + 1 < numParts) {
            s32& nextPart = parts[i + 1];
            nextPart = shp.toInt32(recordContent + 44 + (i + 1) * 4, elittle);
            currPartPoints = nextPart - currPart;
          } else {
            currPartPoints = numPoints - currPart;
          }

          for (u32 j = currPart; j < currPart + currPartPoints; j++) {
            double x = shp.toDouble(pointsPtr + 0 + 16 * j , elittle);
            double y = shp.toDouble(pointsPtr + 8 + 16 * j , elittle);

            vec3 coord = {vec3(glm::radians(x + 90.0), glm::radians(y), 0.f)};
            vertices.push_back({coord});
            indices.push_back(idx);
            if (j - currPart && j + 1 < currPart + currPartPoints)
              indices.push_back(idx);

            idx++;
          }
        }

        recordOffset += 8 + contentLength * 2;
      }

      break;
    }
    default:
      error("[Mesh] Unhandled shape type");
  }

  return Mesh<Vertex1>(vertices, indices, GL_LINES, false);
}

