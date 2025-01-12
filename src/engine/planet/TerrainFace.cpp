#include "TerrainFace.hpp"

#include "glm/geometric.hpp"

#include <vector>

TerrainFace::TerrainFace() {}

TerrainFace::TerrainFace(u16 resolution, vec3 localUp, vec3 color)
  : resolution(resolution),
    localUp(localUp),
    color(color),
    axisA(vec3(localUp.y, localUp.z, localUp.x)),
    axisB(cross(localUp, axisA)),
    mesh(constructMesh()) {}

Mesh TerrainFace::constructMesh() {
  std::vector<Vertex> vertices(resolution * resolution);
  std::vector<GLuint> indices((resolution - 1) * (resolution - 1) * 2 * 3);
  u32 triIndex = 0;

  for (u32 y = 0; y < resolution; y++) {
    for (u32 x = 0; x < resolution; x++) {
      u32 idx = x + y * resolution;
      vec2 percent = vec2(x, y) / (resolution - 1.f);
      vec3 point = localUp + (percent.x - 0.5f) * 2.f * axisA + (percent.y - 0.5f) * 2.f * axisB; // Point on plane
      vertices[idx] = {point, color};
      vertices[idx].normal = localUp;
      vertices[idx].position /= length(point); // Point on sphere

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

void TerrainFace::draw(const Camera& c, const Shader& s) const { mesh.draw(c, s); }
