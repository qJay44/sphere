#include "Light.hpp"

#include "../engine/mesh/meshes.hpp"

Light::Light(vec3 position, float size, vec4 color) : Mesh(meshes::cube(position, size, color)), position(position), color(color) {}

const vec3& Light::getPosition() const { return position; }
const vec4& Light::getColor() const { return color; }
