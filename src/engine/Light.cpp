#include "Light.hpp"

#include "mesh/meshes.hpp"

Light::Light(vec3 position, vec4 color) : Mesh(meshes::cube(position, color)), position(position), color(color) {}

const vec3& Light::getPosition() const { return position; }
const vec4& Light::getColor() const { return color; }
