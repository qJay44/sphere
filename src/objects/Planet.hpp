#pragma once

#include "../engine/Camera.hpp"
#include "../engine/mesh/texture/Texture.hpp"

class Planet {
public:
  Planet(u32 resolution, u32 chunksPerFace, float radius, const fspath& texturePath);
  ~Planet();

  const u32&   getResolution()     const;
  const float& getRadius()         const;
  const float& getHeightmapScale() const;
  const float& getSeaLevel()       const;

  void setHeightmapScale(const float& n);
  void setSeaLevel(const float& n);

  void rebuild();
  void rebuild(int resolution, float radius);
  void draw(const Camera* camera, const Shader& shader) const;

  void _generateNormalMaps();

private:
  friend struct gui;
  friend struct TerrainFace;

  u32 resolution;
  u32 chunks;
  float radius;
  float heightmapScale = 0.225f;
  float seaLevel = 0.f;
  struct TerrainFace* terrainFaces = nullptr;
  Texture heightmaps[2];

  bool colorChunksInsteadOfFaces = true;

private:
  void build();
  vec3 pointOnSphereDefault(const vec3& v) const;
  vec3 pointOnSphereFancy(const vec3& v) const;
};
