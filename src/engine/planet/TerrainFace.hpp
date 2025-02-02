#pragma once

#include "../mesh/Mesh.hpp"

class TerrainFace {
public:
  TerrainFace(const u16& resolution, const vec3& localUp, const vec3& color, const float& radius);

  void add(const Texture& texture);

  void draw(const Camera& camera, const Shader& shader) const;

private:
  const u16& resolution;
  const vec3& localUp;
  const vec3& color;
  const float& radius;
  Mesh mesh;

private:
  Mesh build() const;

  vec3 pointOnSphereDefault(const vec3& v) const;
  vec3 pointOnSphereFancy(const vec3& v) const;
};
