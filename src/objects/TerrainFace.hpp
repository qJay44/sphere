#pragma once

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <list>

#include "TerrainFaceChunk.hpp"
#include "../engine/frustum/Frustum.hpp"
#include "../engine/frustum/volumes/Sphere.hpp"
#include "../engine/CameraStorage.hpp"

struct TerrainFace {
  std::list<TerrainFaceChunk> chunks;
  u32 chunksAmount;
  float heightmapScaleInv;

  TerrainFace() {}

  TerrainFace(const vec3& localUp, const Planet* planet, const vec3& color = vec3(0.f)) {

    chunksAmount = planet->chunks;
    heightmapScaleInv = 1.f / planet->heightmapScale;

    u32 chunksAmountSq = sqrt(chunksAmount);
    u32 chunkResolution = planet->resolution / chunksAmountSq;
    bool randomColor = glm::length(color) == 0.f;

    for (u32 y = 0; y < chunksAmountSq; y++) {
      u32 ystart = y * chunkResolution;
      for (u32 x = 0; x < chunksAmountSq; x++) {
        u32 xstart = x * chunkResolution;
        vec3 col = color;

        if (randomColor) {
          col = {
            (rand() % 256) / 255.f,
            (rand() % 256) / 255.f,
            (rand() % 256) / 255.f
          };
        }

        chunks.push_back(TerrainFaceChunk::build(localUp, planet, chunkResolution, {xstart, ystart}, col));
      }
    }
  }

  ~TerrainFace() {
    for (TerrainFaceChunk& chunk : chunks)
      chunk.clear();
  }

  void draw(const Camera* camera, const Shader& shader) const {
    frustum::Frustum tfFrustum(*CameraStorage::cameraAirplanePtr);

    for (const TerrainFaceChunk& chunk : chunks) {
      vec3 center = (chunk.lastVertex + chunk.firstVertex) * 0.5f;
      float radius = glm::length(chunk.lastVertex - chunk.firstVertex) * heightmapScaleInv;
      frustum::Sphere frustumSphere(center, radius);

      if (frustumSphere.isOnFrustum(tfFrustum, chunk))
        chunk.draw(camera, shader);
    }
  }
};

