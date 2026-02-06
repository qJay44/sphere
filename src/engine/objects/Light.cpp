#include "Light.hpp"

#include "../mesh/meshes.hpp"

Light::Light(vec3 position, float radius, vec3 color)
  : Mesh(meshes::plane(2, GL_TRIANGLES, global::forward)), position(position), radius(radius), color(color) {}

const vec3& Light::getPosition() const { return position; }
const vec3& Light::getColor() const { return color; }
const vec3& Light::getWaveLengths() const { return waveLengths; }

void Light::update() {
  setTrans(position);
}

void Light::draw(const Camera* camera, Shader& shader, bool forceNoWireframe) const {
  shader.setUniform1f("u_radius", radius);
  shader.setUniform3f("u_pos", position);
  shader.setUniform3f("u_col", color);

  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Mesh::draw(camera, shader, forceNoWireframe);

  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
}

