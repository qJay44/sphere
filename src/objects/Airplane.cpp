#include "Airplane.hpp"

#include "../engine/mesh/meshes.hpp"

Airplane::Airplane(
  const Earth& planet,
  vec3 position,
  float speedRad,
  float flyHeight,
  float turnSpeedRad,
  float meshScale
) :
  planet(planet),
  position(position),
  speedRad(speedRad),
  flyHeight(flyHeight),
  turnSpeedRad(turnSpeedRad),
  Mesh(Mesh::loadObj("res/obj/11804_Airplane_v2_l2.obj")) {

  // NOTE: The obj must face -Z initially
  // Model's initial directions
  forward = {-1.f, 0.f, 0.f};
  up      = { 0.f, 0.f, 1.f};
  right   = { 0.f, 1.f, 0.f};

  // Facing +Y
  Mesh::translate(position);
  Mesh::rotate(PI, {0.f, 1.f, 0.f});
  Mesh::rotate(PI_2, {-1.f, 0.f, 0.f});
  Mesh::scale(meshScale);

  // Model's after rotation directions
  forward = { 0.f, 1.f, 0.f};
  up      = { 0.f, 0.f, 1.f};
  right   = { 1.f, 0.f, 0.f};
}

Airplane::~Airplane() {
  Mesh::clear();
}

const vec3&  Airplane::getPosition() const { return position; }
const vec3&  Airplane::getForward()  const { return forward;  }
const vec3&  Airplane::getUp()       const { return up;       }
const vec3&  Airplane::getRight()    const { return right;    }
const float& Airplane::getSpeed()    const { return speedRad; }

vec3 Airplane::getBack() const { return -forward; }
vec3 Airplane::getDown() const { return -up;      }
vec3 Airplane::getLeft() const { return -right;   }

void Airplane::turn(float dir) {
  turnMomentumRad += turnSpeedRad * dir * global::dt;
  if (abs(tiltRecoverMomentumRad) < PI_3)
    tiltMomentumRad += turnMomentumRad;
}

void Airplane::update() {
  // Turn
  glm::quat q = glm::angleAxis(turnMomentumRad, up);
  Mesh::rotate(q);
  right = q * right;
  forward = rotation[2];

  // Tilt
  Mesh::rotate(glm::angleAxis(tiltMomentumRad, -forward));
  tiltRecoverMomentumRad += tiltMomentumRad;

  // Move forward
  float frameSpeedRad = speedRad * global::dt;
  vec3 newPos = normalize(position) + forward * frameSpeedRad;
  vec3 gravityUp = normalize(newPos);
  newPos = gravityUp * (planet.getRadius() + flyHeight);

  Mesh::translate(newPos - position);
  // Finding left since local left (-right) can be tilted up or down
  Mesh::rotate(glm::angleAxis(frameSpeedRad, normalize(cross(up, forward))));

  // Recovering from tilt
  float tiltRecover = tiltRecoverMomentumRad * tiltRecoverMomentumDecreaseFactor;
  Mesh::rotate(glm::angleAxis(tiltRecover, forward));
  tiltRecoverMomentumRad -= tiltRecover;

  up = gravityUp;
  right = -rotation[0];
  forward = normalize(cross(up, right));
  position = newPos;
  turnMomentumRad *= turnMomentumDecreaseFactor;
  tiltMomentumRad *= tiltMomentumDecreaseFactor;
}


void Airplane::draw(const Camera* camera, const Shader& shader, u32 flags) {
  const Shader& colorShader = Shader::getDefaultShader(SHADER_DEFAULT_TYPE_COLOR_SHADER);

  if (flags & AIRPLANE_FLAG_DRAW_RIGHT)
    meshes::line(position, position + right, global::red).draw(camera, colorShader);

  if (flags & AIRPLANE_FLAG_DRAW_UP)
    meshes::line(position, position + up, global::green).draw(camera, colorShader);

  if (flags & AIRPLANE_FLAG_DRAW_FORWARD)
    meshes::line(position, position + forward, global::blue).draw(camera, colorShader);

  Mesh::draw(camera, shader);
}

