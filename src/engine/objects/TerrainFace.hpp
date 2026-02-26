#pragma once

#include <cassert>
#include <list>

#include "TerrainFaceChunk.hpp"
#include "../frustum/volumes/Sphere.hpp"
#include "TileManager.hpp"

struct TerrainFace {
  std::list<TerrainFaceChunk> chunks;
  vec3 color;
  float uvPadding;

  TerrainFace() = default;

  void build(vec3 up, int chunksPerSide, int resolution, float radius) {
    uvPadding = 1.f / (chunksPerSide + chunksPerSide); // FIXME: Heavy lag with smaller radius
    chunks.clear();

    for (int y = 0; y < chunksPerSide; y++)
      for (int x = 0; x < chunksPerSide; x++)
        chunks.push_back(TerrainFaceChunk::build(up, {x, y}, chunksPerSide, resolution, radius));
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

      vec3 centerPos = (chunk.lastVertex.position + chunk.firstVertex.position) * 0.5f;
      float radius = glm::distance(chunk.lastVertex.position, chunk.firstVertex.position);
      frustum::Sphere frustumSphere(centerPos, radius);

      if (frustumSphere.isOnFrustum(frustum, chunk)) {
        vec2 uv0 = chunk.firstVertex.texture;
        vec2 uv1 = chunk.lastVertex.texture;

        vec2 uvCenter = (uv0 + uv1) * 0.5f;

        vec2 uvMin = max(uvCenter - uvPadding, {0.f, 0.f});
        vec2 uvMax = min(uvCenter + uvPadding, {1.f, 1.f});

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

