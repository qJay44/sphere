#include "Airplane.hpp"

#include "mesh/meshes.hpp"

Airplane::Airplane(vec3 position, vec3 forward, float speedRad, float flyHeight, float meshSize)
  : position(position),
    forward(forward),
    speedRad(speedRad),
    flyHeight(flyHeight),
    Mesh(meshes::cube(position, meshSize, {1.f, 0.64f, 0.f})) {
}

void Airplane::update() {
  vec3 newPos = position + forward * speedRad * global::dt;
  vec3 gravityUp = normalize(newPos);
  newPos = gravityUp * (global::planetRadius + flyHeight);

  vec3 v = newPos - position;
  translate(v);

  position = newPos;
}

