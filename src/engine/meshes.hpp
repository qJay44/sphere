#pragma once

#include "Mesh.hpp"

namespace meshes {
Mesh plane(vec3 position, vec2 size, vec3 color = {1.f, 1.f, 1.f});
Mesh cube(vec3 position, vec3 color = {1.f, 1.f, 1.f});
Mesh sphere(float radius, u16 resolution, vec3 color = {1.f, 1.f, 1.f});
} // namespace meshes
