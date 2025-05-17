#pragma once

#include "Mesh.hpp"

namespace meshes {

Mesh<Vertex4> line(vec3 p1, vec3 p2, vec3 color, bool clearable = true);
Mesh<Vertex4> axis(float size = 1.f, bool clearable = true);
Mesh<Vertex4> plane(vec3 position, vec2 size, vec3 color = {1.f, 1.f, 1.f}, bool clearable = true);
Mesh<Vertex4> cube(vec3 position, float size = 1.f, vec3 color = {1.f, 1.f, 1.f}, bool clearable = true);
Mesh<Vertex4> frustum(const Camera& cam, vec3 color = {1.f, 1.f, 1.f}, bool clearable = true);

} // namespace meshes

