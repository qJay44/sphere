#pragma once

#include "Mesh.hpp"

namespace meshes {

Mesh line(vec3 p1, vec3 p2, vec3 color, bool autoClear = true);
Mesh axis(bool autoClear = true);
Mesh plane(vec3 color = {1.f, 1.f, 1.f}, GLenum mode = GL_TRIANGLES, bool autoClear = true);
Mesh plane(size_t resolution, GLenum mode = GL_TRIANGLES, vec3 up = {0.f, 1.f, 0.f}, bool autoClear = true);
Mesh cube(vec3 color = {1.f, 1.f, 1.f}, bool autoClear = true);
Mesh frustum(const Camera& cam, vec3 color = {1.f, 1.f, 1.f}, bool autoClear = true);

} // namespace meshes

