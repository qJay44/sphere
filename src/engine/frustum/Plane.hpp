#pragma once

struct Plane {
  vec3 normal{0.f, 1.f, 0.f};
  float distance = 0.f;

  float getSignedDistanceToPlane(const vec3& point) const {
    return dot(normal, point) - distance;
  }
};

