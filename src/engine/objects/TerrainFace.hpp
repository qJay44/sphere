#pragma once

#include <cassert>
#include <cmath>
#include <list>

#include "TerrainFaceChunk.hpp"
#include "../frustum/volumes/Sphere.hpp"

struct TerrainFace {
  std::list<TerrainFaceChunk> chunks;
  size_t chunksAmount;
  float heightmapScaleInv;

  TerrainFace() {}

  TerrainFace(const vec3& localUp, const Earth* earth) {

    chunksAmount = earth->chunks;
    heightmapScaleInv = 1.f / earth->heightmapScale;

    u32 chunksAmountSq = sqrt(chunksAmount);
    u32 chunkResolution = earth->resolution / chunksAmountSq;

    for (u32 y = 0; y < chunksAmountSq; y++) {
      u32 ystart = y * chunkResolution;
      for (u32 x = 0; x < chunksAmountSq; x++) {
        u32 xstart = x * chunkResolution;
        chunks.push_back(TerrainFaceChunk::build(localUp, earth, chunkResolution, {xstart, ystart}));
      }
    }
  }

  ~TerrainFace() {
    for (TerrainFaceChunk& chunk : chunks)
      chunk.clear();
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

