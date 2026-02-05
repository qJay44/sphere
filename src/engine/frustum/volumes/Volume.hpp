#pragma once

#include "../Frustum.hpp"
#include "../../mesh/Mesh.hpp"

namespace frustum {

struct Volume {
  virtual bool isOnOrForwardPlane(const Plane& plane) const = 0;
  virtual bool isOnFrustum(const Frustum& Frustum, const Mesh& mesh) const = 0;
};

}

