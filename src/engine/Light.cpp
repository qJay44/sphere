#include "Light.hpp"

#include "meshes.hpp"

Light::Light(vec3 position, vec4 color)
  : mesh(meshes::cube(position, color)), position(position), color(color) {}

const vec3& Light::getPosition() const {
  return position;
}

const vec4& Light::getColor() const {
  return color;
}

void Light::draw(const Camera& camera, const Shader& shader) const {
  mesh.draw(camera, shader);
}

