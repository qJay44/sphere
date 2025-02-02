#pragma once

#include "TerrainFace.hpp"

class Planet {
public:
  Planet(u16 resolution, float radius);
  ~Planet();

  const u16& getResolutino() const;
  const float& getRadius() const;

  void add(const Texture& texture);
  void rebuild(u16 resolution, float radius);
  void draw(const Camera& camera, const Shader& shader) const;

private:
  u16 resolution;
  float radius;
  TerrainFace* terrainFaces = nullptr;

private:
  void build();
};
