#pragma once

#include <cmath>
#include <cstdlib>
#include <list>

#include "TerrainFaceChunk.hpp"
#include "../engine/frustum/Frustum.hpp"
#include "../engine/frustum/volumes/Sphere.hpp"

struct TerrainFace {
  std::list<TerrainFaceChunk> chunks;
  u32 chunksAmount;

  TerrainFace() {}

  TerrainFace(const vec3& localUp, const Planet* planet, const vec3& color = vec3(0.f)) {
    chunksAmount = planet->chunks;

    u32 chunksAmountSq = sqrt(chunksAmount);
    u32 chunkResolution = planet->resolution / chunksAmountSq;

    for (u32 y = 0; y < chunksAmountSq; y++) {
      u32 ystart = y * chunkResolution;
      for (u32 x = 0; x < chunksAmountSq; x++) {
        u32 xstart = x * chunkResolution;
        vec3 col = color;

        if (glm::length(color) == 0.f) {
          col= {
            (rand() % 256) / 255.f,
            (rand() % 256) / 255.f,
            (rand() % 256) / 255.f
          };
        }

        TerrainFaceChunk chunk = TerrainFaceChunk::build(localUp, planet, chunkResolution, {xstart, ystart}, col);
        chunk.add(&planet->textures[0]);
        chunk.add(&planet->textures[1]);
        chunks.push_back(chunk);
      }
    }
  }

  ~TerrainFace() {
    for (TerrainFaceChunk& chunk : chunks)
      chunk.clear();
  }

  void draw(const Camera* camera, const Shader& shader) const {
    frustum::Frustum tfFrustum(*camera);

    for (const TerrainFaceChunk& chunk : chunks) {
      // frustum::Sphere frustumSphere(chunk.lastVertex - chunk.firstVertex, 100.f);

      // if (frustumSphere.isOnFrustum(tfFrustum, chunk))
        chunk.draw(camera, shader);
    }
  }
};
