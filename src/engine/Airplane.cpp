#include "Airplane.hpp"

#include "mesh/meshes.hpp"

Airplane::Airplane(vec3 position, vec3 forward, float speedRad, float flyHeight, float meshSize)
  : position(position),
    forward(forward),
    speedRad(speedRad),
    flyHeight(flyHeight),
    Mesh(meshes::cube(position, meshSize, {1.f, 0.64f, 0.f})) {
  rotate({-1.f, 0.f, 0.f}, PI_2); // Facing +Y
  up = {0.f, 0.f, 1.f};
}

const vec3& Airplane::getPosition() const { return position; }
const vec3& Airplane::getUp() const { return up; }

void Airplane::update() {
  float frameSpeedRad = speedRad * global::dt;
  vec3 newPos = normalize(position) + forward * frameSpeedRad;
  vec3 gravityUp = normalize(newPos);
  newPos = gravityUp * (global::planetRadius + flyHeight);

  translate(newPos - position);
  rotate({-1.f, 0.f, 0.f}, frameSpeedRad);

  forward = {rotation[2][0], rotation[2][1], rotation[2][2]};
  up = gravityUp;
  position = newPos;
}

void Airplane::draw(const Camera* camera, const Shader& shader, int flags) {
  if (flags & AIRPLANE_FLAG_DRAW_FORWARD)
    meshes::line(position, position + forward, {1.f, 0.f, 0.f}).draw(camera, shader);

  if (flags & AIRPLANE_FLAG_DRAW_UP)
    meshes::line(position, position + up, {0.f, 1.f, 0.f}).draw(camera, shader);

  Mesh::draw(camera, shader);
}

