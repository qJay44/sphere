#pragma once

#include <cassert>
#include <list>

#include "TerrainFaceChunk.hpp"
#include "../frustum/volumes/Sphere.hpp"

struct TerrainFace {
  std::list<TerrainFaceChunk> chunks;

  TerrainFace() = default;

  TerrainFace(int i, int chunksPerSide, int resolution, float radius) {
    constexpr vec3 directions[6] {
      {1.f,  0.f,  0.f},  // Right
      {-1.f, 0.f,  0.f},  // Left
      {0.f,  1.f,  0.f},  // Top
      {0.f,  -1.f, 0.f},  // Bottom
      {0.f,  0.f,  1.f},  // Back
      {0.f,  0.f,  -1.f}, // Front
    };

    for (int y = 0; y < chunksPerSide; y++)
      for (int x = 0; x < chunksPerSide; x++)
        chunks.push_back(TerrainFaceChunk::build(directions[i], {x, y}, chunksPerSide, resolution, radius));
  }

  void draw(const Camera* camera, Shader& shader, frustum::Frustum frustum) const {
    for (const TerrainFaceChunk& chunk : chunks) {
      shader.setUniform3f("u_terrainFaceChunkColor", chunk.debugColor);

      vec3 center = (chunk.lastVertex + chunk.firstVertex) * 0.5f;
      float radius = glm::length(chunk.lastVertex - chunk.firstVertex) * 2.f; // Additionally multipling by 2 to keep some chunks when camera is to close to the earth
      frustum::Sphere frustumSphere(center, radius);

      if (frustumSphere.isOnFrustum(frustum, chunk))
        chunk.draw(camera, shader);
    }
  }
};

