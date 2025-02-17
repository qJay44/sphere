#pragma once

#include "../Mesh.hpp"

class Planet {
public:
  Planet(u16 resolution, float radius, const fspath& path);

  const u16& getResolutino() const;
  const float& getRadius() const;

  void rebuild(u16 resolution, float radius);
  void draw(const Camera& camera, const Shader& shader) const;

private:
  u16 resolution;
  float radius;
  Mesh terrainFaces[6];
  Texture textures[2];

private:
  void build();
  vec3 pointOnSphereDefault(const vec3& v) const;
  vec3 pointOnSphereFancy(const vec3& v) const;
};
