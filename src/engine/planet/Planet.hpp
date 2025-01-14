#pragma once

#include "TerrainFace.hpp"
#include <string>

class Planet {
public:
  Planet(u32 resolution);

  void readGeoData(const std::string& path) const;
  void draw(const Camera& camera, const Shader& shader) const;

private:
  u32 resolution;
  TerrainFace terrainFaces[6];
};
