#pragma once

#include "../engine/mesh/Mesh.hpp"

#include "Planet.hpp"

struct TerrainFaceChunk : public Mesh {
  vec3 firstVertex;
  vec3 lastVertex;

  static TerrainFaceChunk build(
    const vec3& up,
    const Planet* planet,
    const u32& size,
    const ivec2& start,
    const vec3& color = vec3(1.f)
  ) {
    const u32& resolution = planet->getResolution();

    bool extraColumn = resolution - start.x != size;
    bool extraRow    = resolution - start.y != size;

    u32 resolutionX = size + extraColumn;
    u32 resolutionY = size + extraRow;

    std::vector<Vertex> vertices(resolutionX * resolutionY);
    std::vector<GLuint> indices(resolutionX * resolutionY * 2 * 3);
    vec3 axisA = vec3(up.y, up.z, up.x);
    vec3 axisB = cross(up, axisA);
    u32 triIndex = 0;

    for (u32 y = 0; y < resolutionY; y++) {
      float percentY = (start.y + y) / (resolution - 1.f);
      vec3 pY = (percentY - 0.5f) * 2.f * axisB;

      for (u32 x = 0; x < resolutionX; x++) {
        u32 idx = x + y * resolutionX;
        float percentX = (start.x + x) / (resolution - 1.f);
        vec3 pX = (percentX - 0.5f) * 2.f * axisA;
        vec3 pointOnPlane = up + pX + pY;
        Vertex& vertex = vertices[idx];

        vertex = {pointOnSphereFancy(pointOnPlane), color};
        vertex.normal = vertex.position;
        vertex.texture = {percentX, percentY};
        vertex.position *= planet->getRadius();

        if (x != resolutionX - 1 && y != resolutionY - 1) {
          indices[triIndex + 0] = idx;
          indices[triIndex + 1] = idx + resolutionX + 1;
          indices[triIndex + 2] = idx + resolutionX;

          indices[triIndex + 3] = idx;
          indices[triIndex + 4] = idx + 1;
          indices[triIndex + 5] = idx + resolutionX + 1;

          triIndex += 6;
        }
      }
    }

    return TerrainFaceChunk(vertices, indices);
  }

  TerrainFaceChunk() {}

  TerrainFaceChunk(std::vector<Vertex> vertices, std::vector<GLuint> indices) : Mesh(vertices, indices) {
    clearable = false;
    this->firstVertex = vertices.front().position;
    this->lastVertex = vertices.back().position;
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

