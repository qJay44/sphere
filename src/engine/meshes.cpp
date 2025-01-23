#include "meshes.hpp"

#include <vector>

namespace meshes {
Mesh plane(vec3 position, vec2 size, vec3 color) {
  std::vector<Vertex> vertices{
    {{-0.1f * size.x + position.x, -0.1f * size.y + position.y, position.z}, color},
    {{-0.1f * size.x + position.x,  0.1f * size.y + position.y, position.z}, color},
    {{ 0.1f * size.x + position.x,  0.1f * size.y + position.y, position.z}, color},
    {{ 0.1f * size.x + position.x, -0.1f * size.y + position.y, position.z}, color},
  };

  std::vector<GLuint> indices{
    0, 1, 2,
    2, 3, 0
  };

  return Mesh(vertices, indices);
}

Mesh cube(vec3 position, vec3 color) {
  //        5--------6
  //       /|       /|
  //      1--------2 |
  //      | |      | |
  //      | 4------|-7
  //      |/       |/
  //      0--------3
  //
  //      x: - to left, + to right
  //      y: - to down, + to up
  //      z: - to rear, + to front

  std::vector<Vertex> vertices{
    {{-0.1f + position.x, -0.1f + position.y,  0.1f + position.y}, color},
    {{-0.1f + position.x,  0.1f + position.y,  0.1f + position.y}, color},
    {{ 0.1f + position.x,  0.1f + position.y,  0.1f + position.y}, color},
    {{ 0.1f + position.x, -0.1f + position.y,  0.1f + position.y}, color},
    {{-0.1f + position.x, -0.1f + position.y, -0.1f + position.y}, color},
    {{-0.1f + position.x,  0.1f + position.y, -0.1f + position.y}, color},
    {{ 0.1f + position.x,  0.1f + position.y, -0.1f + position.y}, color},
    {{ 0.1f + position.x, -0.1f + position.y, -0.1f + position.y}, color},
  };

  std::vector<GLuint> indices{
    0, 1, 2, //  Font face
    2, 3, 0, //
    4, 5, 6, //  Rear face
    6, 7, 4, //
    0, 1, 5, //  Left face
    5, 4, 0, //
    3, 2, 6, //  Right face
    6, 7, 3, //
    1, 5, 6, //  Top face
    6, 2, 1, //
    0, 4, 7, //  Bottom face
    7, 3, 0, //
  };

  return Mesh(vertices, indices);
}

// FIXME: Lacking one segment
Mesh sphere(float radius, u16 resolution, vec3 color) {
  std::vector<Vertex> vertices(resolution * resolution);
  std::vector<GLuint> indices((resolution - 1) * (resolution - 1) * 2 * 3);

  float stepLon = (2.f * PI) / resolution; // From -PI to PI, total length = 2PI
  float stepLat = PI / resolution;         // From -PI_2 to PI_2, total length = 2PI_2 -> PI
  u32 triIndex = 0;

  for (u32 i = 0; i < resolution; i++) {
    float lon = i * stepLon;
    for (u32 j = 0; j < resolution; j++) {
      float lat = j * stepLat;
      u32 idx = i + j * resolution;
      vec3 point{
        radius * sinf(lat) * cosf(lon),
        radius * sinf(lat) * sinf(lon),
        radius * cosf(lat),
      };

      vertices[idx] = {point, color, {}, point / radius};

      if (i != resolution - 1 && j != resolution - 1) {
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
} // namespace meshes
