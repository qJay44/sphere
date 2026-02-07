#pragma once

#include "../mesh/Mesh.hpp"

#include <cmath>
#include <cstdlib>

struct TerrainFaceChunk : public Mesh {
  vec3 firstVertex;
  vec3 lastVertex;
  vec3 debugColor;

  static TerrainFaceChunk build(
    vec3 up,
    vec2 start,
    int chunksPerSide,
    int resolution,
    float radius
  ) {
    std::vector<VertexPT> vertices(resolution * resolution);
    std::vector<GLuint> indices((resolution - 1) * (resolution - 1) * 2 * 3);
    size_t triIndex = 0;

    vec3 axisA = vec3(up.y, up.z, up.x);
    vec3 axisB = cross(up, axisA);

    float step = 1.f / (chunksPerSide * (resolution - 1));
    float globalStartY = start.y * (resolution - 1);
    float globalStartX = start.x * (resolution - 1);

    for (int y = 0; y < resolution; y++) {
      float globalY = globalStartY + y;
      float percentY = globalY * step * 2.f - 1.f;
      vec3 pY = percentY * axisB;

      for (int x = 0; x < resolution; x++) {
        float globalX = globalStartX + x;
        float percentX = globalX * step * 2.f - 1.f;
        vec3 pX = percentX * axisA;

        int idx = x + y * resolution;
        vec3 pointOnPlane = up + pX + pY;
        VertexPT& vertex = vertices[idx];
        vertex.position = spherifyFancy(pointOnPlane) * radius;

        vec3 normal = normalize(vertex.position);
        vertex.texture = {
          0.5f - atan2(normal.z, normal.x) / (2.f * PI),
          0.5f + asin(normal.y) / PI
        };

        if (x != resolution - 1 && y != resolution - 1) {
          indices[triIndex + 0] = idx + resolution;      // 2       0 -------- 1
          indices[triIndex + 1] = idx;                   // 0       |          |
          indices[triIndex + 2] = idx + 1;               // 1       |          |
          //                                                        |          |
          indices[triIndex + 3] = idx + 1;               // 1       |          |
          indices[triIndex + 4] = idx + resolution + 1;  // 3       |          |
          indices[triIndex + 5] = idx + resolution;      // 2       2 -------- 3

          triIndex += 6;
        }
      }
    }

    return TerrainFaceChunk(vertices, indices, resolution);
  }

  TerrainFaceChunk(
    const std::vector<VertexPT>& vertices,
    const std::vector<GLuint>& indices,
    const size_t& resolution
  ) : Mesh(vertices, indices, GL_TRIANGLES) {
    firstVertex = vertices.front().position;
    lastVertex = vertices.back().position;
    debugColor = {
      (rand() % 255) / 255.f,
      (rand() % 255) / 255.f,
      (rand() % 255) / 255.f
    };
  }

private:
  static vec3 spherify(const vec3& v) { return v / length(v); }

  static vec3 spherifyFancy(const vec3& v) {
    float x2 = v.x * v.x;
    float y2 = v.y * v.y;
    float z2 = v.z * v.z;

    return {
      v.x * sqrtf(1.f - (y2 + z2) * 0.5f + (y2 * z2) / 3.f),
      v.y * sqrtf(1.f - (z2 + x2) * 0.5f + (z2 * x2) / 3.f),
      v.z * sqrtf(1.f - (x2 + y2) * 0.5f + (x2 * y2) / 3.f),
    };
  }
};

