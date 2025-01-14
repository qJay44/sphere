#pragma once

#include "../Mesh.hpp"

class TerrainFace {
public:
  TerrainFace();
  TerrainFace(u16 resolution, vec3 localUp, vec3 color = {1.f, 1.f, 1.f});

  void draw(const Camera& camera, const Shader& shader) const;

private:
  u16 resolution;
  vec3 localUp;
  vec3 axisA;
  vec3 axisB;
  vec3 color;

  Mesh mesh;

private:
  Mesh constructMesh();

  vec3 pointOnSphereDefault(const vec3& v) const;
  vec3 pointOnSphereFancy(const vec3& v) const;
};
