#include "TerrainFace.hpp"

#include <cmath>
#include <vector>

#include "glm/geometric.hpp"

TerrainFace::TerrainFace() {}

TerrainFace::TerrainFace(u16 resolution, vec3 localUp, const GEBCO* data, vec3 color)
  : resolution(resolution),
    localUp(localUp),
    color(color),
    axisA(vec3(localUp.y, localUp.z, localUp.x)),
    axisB(cross(localUp, axisA)),
    mesh(constructMesh(data)) {}

Mesh TerrainFace::constructMesh(const GEBCO* data) const {
  std::vector<Vertex> vertices(resolution * resolution);
  std::vector<GLuint> indices((resolution - 1) * (resolution - 1) * 2 * 3);
  u32 triIndex = 0;

  for (u32 y = 0; y < resolution; y++) {
    float percentY = static_cast<float>(y) / (resolution - 1);
    vec3 pY = (percentY - 0.5f) * 2.f * axisB;

    for (u32 x = 0; x < resolution; x++) {
      u32 idx = x + y * resolution;
      float percentX = static_cast<float>(x) / (resolution - 1.f);
      vec3 pX = (percentX - 0.5f) * 2.f * axisA;
      vec3 point = localUp + pX + pY; // Point on plane
      Vertex& vertex = vertices[idx];

      vertex = {pointOnSphereFancy(point), color};
      vertex.normal = vertex.position;

      float lat = asin(vertex.position.y);
      float lon = atan2(-vertex.position.z, vertex.position.x);
      short elevation = data->elevation(lat, lon);
      vertex.position += vertex.normal * (elevation * 0.000025f);
      vertex.position *= 2.f;

      if (x != resolution - 1 && y != resolution - 1) {
        indices[triIndex + 0] = idx;
        indices[triIndex + 1] = idx + resolution + 1;
        indices[triIndex + 2] = idx + resolution;

        indices[triIndex + 3] = idx;
        indices[triIndex + 4] = idx + 1;
        indices[triIndex + 5] = idx + resolution + 1;
        triIndex += 6;
      }
    }
  }

  return Mesh(vertices, indices);
}

void TerrainFace::add(const Texture& texture) { mesh.add(texture); }

void TerrainFace::draw(const Camera& c, const Shader& s) const { mesh.draw(c, s); }

vec3 TerrainFace::pointOnSphereDefault(const vec3& v) const { return v / length(v); }

vec3 TerrainFace::pointOnSphereFancy(const vec3& v) const {
  float x2 = v.x * v.x;
  float y2 = v.y * v.y;
  float z2 = v.z * v.z;
  float x = v.x * sqrtf(1.f - (y2 + z2) * 0.5f + (y2 * z2) / 3.f);
  float y = v.y * sqrtf(1.f - (z2 + x2) * 0.5f + (z2 * x2) / 3.f);
  float z = v.z * sqrtf(1.f - (x2 + y2) * 0.5f + (x2 * y2) / 3.f);

  return vec3(x, y, z);
}
