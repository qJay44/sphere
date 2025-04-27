#include "Airplane.hpp"

#include <glm/gtx/string_cast.hpp>
#include "mesh/meshes.hpp"

Airplane::Airplane(vec3 position, vec3 forward, float speedRad, float flyHeight, float meshSize)
  : position(position),
    forward(forward),
    speedRad(speedRad),
    flyHeight(flyHeight),
    Mesh(meshes::cube(position, meshSize, {1.f, 0.64f, 0.f})) {
  rotate({-1.f, 0.f, 0.f}, PI_2); // Facing +Y
}

void Airplane::update() {
  float frameSpeedRad = speedRad * global::dt;
  vec3 newPos = normalize(position) + forward * frameSpeedRad;
  vec3 gravityUp = normalize(newPos);
  newPos = gravityUp * (global::planetRadius + flyHeight);

  translate(newPos - position);
  rotate({-1.f, 0.f, 0.f}, frameSpeedRad);

  forward = {rotation[2][0], rotation[2][1], rotation[2][2]};
  position = newPos;
}

void Airplane::drawForward(const Camera* camera, const Shader& shader, float size, vec3 color) {
  if (forward.x != 0.f) color = {color.y, color.z, color.x};
  meshes::line(position, position + forward * size, color).draw(camera, shader);
}

