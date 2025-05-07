#pragma once

#include "Mesh.hpp"

namespace meshes {

Mesh line(vec3 p1, vec3 p2, vec3 color, bool clearable = true);
Mesh axis(float size = 1.f, bool clearable = true);
Mesh plane(vec3 position, vec2 size, vec3 color = {1.f, 1.f, 1.f}, bool clearable = true);
Mesh cube(vec3 position, float size = 1.f, vec3 color = {1.f, 1.f, 1.f}, bool clearable = true);
Mesh frustum(const Camera& cam, vec3 color = {1.f, 1.f, 1.f}, bool clearable = true);

} // namespace meshes

