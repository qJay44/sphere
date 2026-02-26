#pragma once

#include "../mesh/Mesh.hpp"

#include <cmath>
#include <cstdlib>

struct TerrainFaceChunk : public Mesh {
  VertexPT firstVertex;
  VertexPT lastVertex;
  vec3 debugColor;

  static TerrainFaceChunk build(
    vec3 up,
    vec2 start,
    int chunksPerSide,
    int resolution,
    float radius
  ) {
    std::vector<VertexPT> vertices(resolution * resolution);
    std::vector<GLuint> indices((resolution - 1) * (resolution - 1) * 4);
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
        int idx = x + y * resolution;
        float globalX = globalStartX + x;
        float percentX = globalX * step * 2.f - 1.f;
        vec3 pX = percentX * axisA;
        vec3 pointOnPlane = up + pX + pY;

        VertexPT& vertex = vertices[idx];
        vec3 pos = spherifyFancy(pointOnPlane) * radius;
        vec3 normal = normalize(pos);
        float u = atan2(normal.z, normal.x) / (2.f * PI) + 0.5f;
        float v = asin(normal.y) / PI + 0.5f;

        // To avoid using vips flipping operations
        u = 1.f - u;
        v = 1.f - v;

        if (x != resolution - 1 && y != resolution - 1) {
          // CCW
          indices[triIndex + 0] = idx + resolution + 1; // 0
          indices[triIndex + 1] = idx + 1;              // 1
          indices[triIndex + 2] = idx;                  // 2
          indices[triIndex + 3] = idx + resolution;     // 3

          triIndex += 4;
        }

        vertex.position = pos;
        vertex.texture = {u, v}; // Just for the TileManager
      }
    }

    return TerrainFaceChunk(vertices, indices, resolution);
  }

  TerrainFaceChunk(
    const std::vector<VertexPT>& vertices,
    const std::vector<GLuint>& indices,
    const size_t& resolution
  ) : Mesh(vertices, indices, GL_PATCHES) {
    firstVertex = vertices.front();
    lastVertex = vertices.back();
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

