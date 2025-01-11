#include "meshes.hpp"

#include <vector>

namespace meshes {
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

    std::vector<Vertex> vertices {
      // Front
      {{-0.1f + position.x, -0.1f + position.y,  0.1f + position.y}, color},
      {{-0.1f + position.x,  0.1f + position.y,  0.1f + position.y}, color},
      {{ 0.1f + position.x,  0.1f + position.y,  0.1f + position.y}, color},
      {{ 0.1f + position.x, -0.1f + position.y,  0.1f + position.y}, color},
      // Rear + position.x
      {{-0.1f + position.x, -0.1f + position.y, -0.1f + position.y}, color},
      {{-0.1f + position.x,  0.1f + position.y, -0.1f + position.y}, color},
      {{ 0.1f + position.x,  0.1f + position.y, -0.1f + position.y}, color},
      {{ 0.1f + position.x, -0.1f + position.y, -0.1f + position.y}, color},
    };

    std::vector<GLuint> indices {
      // Front face
      0, 1, 2,
      2, 3, 0,
      // Rear face
      4, 5, 6,
      6, 7, 4,
      // Left face
      0, 1, 5,
      5, 4, 0,
      // Right face
      3, 2, 6,
      6, 7, 3,
      // Top face
      1, 5, 6,
      6, 2, 1,
      // Bottom face
      0, 4, 7,
      7, 3, 0
    };

    return Mesh(vertices, indices);
  }

  Mesh sphere(float radius, u16 resolution, vec3 color) {
    static const auto getPos = [&radius](const float& lon, const float& lat) {
      return vec3{
        radius * sinf(lat) * cosf(lon),
        radius * sinf(lat) * sinf(lon),
        radius * cosf(lat)
      };
    };

    std::vector<Vertex> vertices;

    float stepLon = (2.f * PI) / resolution; // From -PI to PI, total length = 2PI
    float stepLat = PI / resolution;         // From -PI_2 to PI_2, total length = 2PI_2 -> PI

    for (int i = 0; i < resolution; i++) {
      float lon1 = i * stepLon;
      float lon2 = i == resolution - 1 ? 2.f * PI : (i + 1) * stepLon;
      for (int j = 0; j < resolution; j++) {
        float lat1 = j * stepLat;
        float lat2 = j == resolution - 1 ? PI : (j + 1) * stepLat;

        Vertex p0{getPos(lon1, lat1), color};
        Vertex p1{getPos(lon1, lat2), color};
        Vertex p2{getPos(lon2, lat1), color};
        Vertex p3{getPos(lon2, lat2), color};

        p0.normal = p0.postion / radius;
        p1.normal = p1.postion / radius;
        p2.normal = p2.postion / radius;
        p3.normal = p3.postion / radius;

        vertices.push_back(p2);
        vertices.push_back(p0);
        vertices.push_back(p1);

        vertices.push_back(p2);
        vertices.push_back(p1);
        vertices.push_back(p3);
      }
    }

    return Mesh(vertices);
  }
} // meshes

