#include "Sphere.hpp"

namespace frustum {

Sphere::Sphere(const vec3& inCenter, const float& inRadius) : Volume{}, center(inCenter), radius(inRadius) {}

bool Sphere::isOnOrForwardPlane(const Plane& plane) const {
  return plane.getSignedDistanceToPlane(center) > -radius;
}

bool Sphere::isOnFrustum(const Frustum& frustum, const Mesh& mesh) const {
  mat4 model = mesh.getModel();
  vec3 globalScale = {glm::length(model[0]), glm::length(model[1]), glm::length(model[2])};
  vec3 globalCenter = model * vec4(center, 1.f);
  float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

  Sphere globalSphere{globalCenter, radius * (maxScale * 0.5f)};

  return (
    globalSphere.isOnOrForwardPlane(frustum.leftFace) &&
    globalSphere.isOnOrForwardPlane(frustum.rightFace) &&
    globalSphere.isOnOrForwardPlane(frustum.farFace) &&
    globalSphere.isOnOrForwardPlane(frustum.nearFace) &&
    globalSphere.isOnOrForwardPlane(frustum.topFace) &&
    globalSphere.isOnOrForwardPlane(frustum.bottomFace)
  );
}

}; // namespace frustum

