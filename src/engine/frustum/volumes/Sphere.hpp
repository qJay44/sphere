#pragma once

#include "Volume.hpp"

namespace frustum {

struct Sphere : public Volume {
  static float radiusMultiplier;

  vec3 center = vec3(0.f);
  float radius = 0.f;

  Sphere(const vec3& inCenter, const float& inRadius);

  bool isOnOrForwardPlane(const Plane& plane);
  bool isOnFrustum(const Frustum& frustum, const Mesh& mesh) const final;
};

}

