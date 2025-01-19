#pragma once

#include "TerrainFace.hpp"

class Planet {
public:
  Planet(u32 resolution);

  void add(const Texture& texture);

  void draw(const Camera& camera, const Shader& shader) const;

private:
  u32 resolution;
  TerrainFace terrainFaces[6];
};
