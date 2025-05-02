#include "Airplane.hpp"

#include "../engine/mesh/meshes.hpp"

Airplane::Airplane(const Planet& planet, vec3 position, float speedRad, float flyHeight, float meshSize)
  : planet(planet),
    position(position),
    speedRad(speedRad),
    flyHeight(flyHeight),
    Mesh(meshes::cube(position, meshSize, {1.f, 0.64f, 0.f})) {

  // Facing +Y
  Mesh::rotate(PI_2, rotAxis);
  forward = {0.f, 1.f, 0.f};
  up      = {0.f, 0.f, 1.f};
  right   = {1.f, 0.f, 0.f};
}

const vec3&      Airplane::getPosition()   const { return position; }
const vec3&      Airplane::getForward()    const { return forward;  }
const vec3&      Airplane::getUp()         const { return up;       }
const vec3&      Airplane::getRight()      const { return right;    }
const float&     Airplane::getSpeed()      const { return speedRad; }
const glm::quat& Airplane::getQuaternion() const { return quat;     }

vec3 Airplane::getBack() const { return -forward; }
vec3 Airplane::getDown() const { return -up;      }
vec3 Airplane::getLeft() const { return -right;   }

void Airplane::update() {
  float frameSpeedRad = speedRad * global::dt;
  vec3 newPos = normalize(position) + forward * frameSpeedRad;
  vec3 gravityUp = normalize(newPos);
  newPos = gravityUp * (planet.getRadius() + flyHeight);

  glm::quat quat = glm::angleAxis(frameSpeedRad, rotAxis);
  Mesh::translate(newPos - position);
  Mesh::rotate(quat);

  forward = rotation[2];
  up = gravityUp;
  right = rotation[0];
  position = newPos;
}

void Airplane::draw(const Camera* camera, const Shader& shader, u32 flags) {
  if (flags & AIRPLANE_FLAG_DRAW_FORWARD)
    meshes::line(position, position + forward, {1.f, 0.f, 0.f}).draw(camera, shader);

  if (flags & AIRPLANE_FLAG_DRAW_UP)
    meshes::line(position, position + up, {0.f, 1.f, 0.f}).draw(camera, shader);

  if (flags & AIRPLANE_FLAG_DRAW_RIGHT)
    meshes::line(position, position + right, {0.f, 0.f, 1.f}).draw(camera, shader);

  Mesh::draw(camera, shader);
}

