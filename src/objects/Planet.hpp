#pragma once

#include "../engine/mesh/Mesh.hpp"

class Planet {
public:
  Planet(u16 resolution, float radius, const fspath& texturePath);

  const u16& getResolution() const;
  const float& getRadius() const;
  const float& getHeightmapScale() const;
  const float& getSeaLevel() const;

  void setHeightmapScale(const float& n);
  void setSeaLevel(const float& n);

  void rebuild(u16 resolution, float radius);
  void draw(const Camera& camera, const Shader& shader) const;

private:
  u16 resolution;
  float radius;
  float heightmapScale = 0.225f;
  float seaLevel = 0.f;
  Mesh terrainFaces[6];
  Texture textures[2];

private:
  void build();
  vec3 pointOnSphereDefault(const vec3& v) const;
  vec3 pointOnSphereFancy(const vec3& v) const;
};
