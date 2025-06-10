#pragma once

#include <cassert>
#include <cmath>
#include <list>

#include "TerrainFaceChunk.hpp"
#include "../engine/frustum/Frustum.hpp"
#include "../engine/frustum/volumes/Sphere.hpp"
#include "../engine/CameraStorage.hpp"

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

  void draw(const Camera* camera, const Shader& shader, const vec3& tfColor) const {
    static const GLint tfColorLoc = shader.getUniformLoc("u_debug_terrainFaceColor");
    static const GLint tfChunkColorLoc = shader.getUniformLoc("u_debug_terrainFaceChunkColor");

    shader.setUniform3f(tfColorLoc, tfColor);

    frustum::Frustum tfFrustum(*CameraStorage::cameraAirplanePtr);

    for (const TerrainFaceChunk& chunk : chunks) {
      shader.setUniform3f(tfChunkColorLoc, chunk.debug_color);

      vec3 center = (chunk.lastVertex + chunk.firstVertex) * 0.5f;
      float radius = glm::length(chunk.lastVertex - chunk.firstVertex) * 2.f; // Additionally multipling by 2 to keep some chunks when camera is to close to the earth
      frustum::Sphere<VertexPT> frustumSphere(center, radius);

      if (frustumSphere.isOnFrustum(tfFrustum, chunk))
        chunk.draw(camera, shader);
    }
  }
};

