#pragma once

#include "TerrainFaceChunk.hpp"
#include "TileManager.hpp"
#include "../frustum/volumes/Sphere.hpp"

struct TerrainFace {
  std::list<TerrainFaceChunk> chunks;
  vec3 color;

  TerrainFace() = default;

  void build(vec3 up, int chunksPerSide, int resolution, float radius) {
    chunks.clear();

    for (int y = 0; y < chunksPerSide; y++)
      for (int x = 0; x < chunksPerSide; x++)
        chunks.push_back(TerrainFaceChunk(up, {x, y}, chunksPerSide, resolution, radius));
  }

  void draw(const Camera* camera, Shader& shader) const {
    for (const TerrainFaceChunk& chunk : chunks)
      chunk.draw(camera, shader);
  }

  int draw(const Camera* camera, Shader& shader, const frustum::Frustum& frustum, TileManager& tm) const {
    int cnt = 0;
    shader.setUniform3f("u_terrainFaceColor", color);

    for (const TerrainFaceChunk& chunk : chunks) {
      shader.setUniform3f("u_terrainFaceChunkColor", chunk.debugColor);

      vec3 centerPos = (chunk.firstPos + chunk.lastPos) * 0.5f;
      float radius = glm::distance(chunk.lastPos, chunk.firstPos);
      frustum::Sphere frustumSphere(centerPos, radius);

      if (frustumSphere.isOnFrustum(frustum, chunk)) {
        vec2 uvMin = chunk.minUV;
        vec2 uvMax = chunk.minUV;
        constexpr float epsilon = 0.01f;

        uvMin = max(uvMin - epsilon, {0.f, 0.f});
        uvMax = min(uvMax + epsilon, {1.f, 1.f});

        if (uvMin.y < 0.001f || uvMax.y > 0.999f) {
          uvMin.x = 0.f;
          uvMax.x = 1.f;
        }

        tm.requestTile(uvMin, uvMax);

        chunk.draw(camera, shader);
        cnt++;
      }
    }

    return cnt;
  }
};

