#include "cube.hpp"

#include <vector>

Mesh templateCube(vec3 color) {
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

  std::vector<float> vertices {
    // Front
   -0.1f, -0.1f,  0.1f,   color.x, color.y, color.z,   0.f, 0.f,   0.f, 0.f, 0.f,
   -0.1f,  0.1f,  0.1f,   color.x, color.y, color.z,   0.f, 0.f,   0.f, 0.f, 0.f,
    0.1f,  0.1f,  0.1f,   color.x, color.y, color.z,   0.f, 0.f,   0.f, 0.f, 0.f,
    0.1f, -0.1f,  0.1f,   color.x, color.y, color.z,   0.f, 0.f,   0.f, 0.f, 0.f,
    // Rear
   -0.1f, -0.1f, -0.1f,   color.x, color.z, color.y,   0.f, 0.f,   0.f, 0.f, 0.f,
   -0.1f,  0.1f, -0.1f,   color.x, color.z, color.y,   0.f, 0.f,   0.f, 0.f, 0.f,
    0.1f,  0.1f, -0.1f,   color.x, color.z, color.y,   0.f, 0.f,   0.f, 0.f, 0.f,
    0.1f, -0.1f, -0.1f,   color.x, color.z, color.y,   0.f, 0.f,   0.f, 0.f, 0.f,
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
