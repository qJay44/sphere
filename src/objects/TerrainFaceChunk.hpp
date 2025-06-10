#pragma once

#include "../engine/mesh/Mesh.hpp"

#include "Earth.hpp"
#include <cmath>
#include <cstdlib>

struct TerrainFaceChunk : public Mesh<VertexPT> {
  vec3 firstVertex;
  vec3 lastVertex;
  vec3 debug_color;

  static TerrainFaceChunk build(
    const vec3& up,
    const Earth* earth,
    const size_t& size,
    const ivec2& start
  ) {
    const size_t& resolution = earth->getResolution();

    bool extraColumn = resolution - start.x != size;
    bool extraRow    = resolution - start.y != size;

    size_t resolutionX = size + extraColumn + 1;
    size_t resolutionY = size + extraRow;

    std::vector<VertexPT> vertices(resolutionX * resolutionY);
    std::vector<GLuint> indices(resolutionX * resolutionY * 2 * 3);
    vec3 axisA = vec3(up.y, up.z, up.x);
    vec3 axisB = cross(up, axisA);
    size_t triIndex = 0;

    for (size_t y = 0; y < resolutionY; y++) {
      float percentY = (start.y + y) / (resolution - 1.f);
      vec3 pY = (percentY - 0.5f) * 2.f * axisB;

      for (size_t x = 0; x < resolutionX; x++) {
        size_t idx = x + y * resolutionX;
        float percentX = (start.x + x) / (resolution - 1.f);
        vec3 pX = (percentX - 0.5f) * 2.f * axisA;
        vec3 pointOnPlane = up + pX + pY;
        VertexPT& vertex = vertices[idx];
        vertex.position = pointOnSphereFancy(pointOnPlane) * earth->getRadius();

        vec3 normal = normalize(vertex.position);
        vertex.texture = {
          0.5f - atan2(normal.z, normal.x) / (2.f * PI),
          0.5f + asin(normal.y) / PI
        };

        if (x != resolutionX - 1 && y != resolutionY - 1) {
          indices[triIndex + 0] = idx + resolutionX;      // 2       0 -------- 1
          indices[triIndex + 1] = idx;                    // 0       |          |
          indices[triIndex + 2] = idx + 1;                // 1       |          |
          //                                                         |          |
          indices[triIndex + 3] = idx + 1;                // 1       |          |
          indices[triIndex + 4] = idx + resolutionX + 1;  // 3       |          |
          indices[triIndex + 5] = idx + resolutionX;      // 2       2 -------- 3

          triIndex += 6;
        }
      }
    }

    return TerrainFaceChunk(vertices, indices, resolutionX);
  }

  TerrainFaceChunk() {}

  TerrainFaceChunk(
    const std::vector<VertexPT>& vertices,
    const std::vector<GLuint>& indices,
    const size_t& resolutionX
  ) : Mesh(vertices, indices, GL_TRIANGLES, false) {
    firstVertex = vertices.front().position;
    lastVertex = vertices.back().position;
    debug_color = {
      (rand() % 255) / 255.f,
      (rand() % 255) / 255.f,
      (rand() % 255) / 255.f
    };
  }

private:
  static vec3 pointOnSphereDefault(const vec3& v) { return v / length(v); }

  static vec3 pointOnSphereFancy(const vec3& v) {
    float x2 = v.x * v.x;
    float y2 = v.y * v.y;
    float z2 = v.z * v.z;
    float x = v.x * sqrtf(1.f - (y2 + z2) * 0.5f + (y2 * z2) / 3.f);
    float y = v.y * sqrtf(1.f - (z2 + x2) * 0.5f + (z2 * x2) / 3.f);
    float z = v.z * sqrtf(1.f - (x2 + y2) * 0.5f + (x2 * y2) / 3.f);

    return vec3(x, y, z);
  }
};

