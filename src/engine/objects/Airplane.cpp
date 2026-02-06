#include "Airplane.hpp"

#include "../mesh/meshes.hpp"
#include "glm/common.hpp"

Texture Airplane::texDiffuse;

Airplane::Airplane(vec3 position, float flyHeight, float meshScale)
  : Moveable(position, 0.f, 0.f),
  // TODO Pass path instead
    Mesh(Mesh::loadObj("res/obj/11804_Airplane_v2_l2.obj")),
    camera(position),
    flyHeight(flyHeight),
    meshScale(meshScale)
{
  if (texDiffuse.getUniformName().empty())
    texDiffuse = Texture("res/tex/airplane/11804_Airplane_diff.jpg", {"diffuse0", 0});

  // Facing +Y
  Mesh::translate(position);
  Mesh::rotate(PI, {0.f, 1.f, 0.f});
  Mesh::rotate(PI_2, {-1.f, 0.f, 0.f});
  Mesh::scale(meshScale);
  setOrientation({0.f, 1.f, 0.f});
  up = {0.f, 0.f, 1.f};

  camera.setPosition(position + getBack() * camDistance);
  camera.setView(this);
  camera.update();
}

Airplane::~Airplane() {
  Mesh::clear();
}

void Airplane::moveLeft()  { turn( 1.f); }
void Airplane::moveRight() { turn(-1.f); }

void Airplane::moveUp()   { speedDefault = 0.f;   }
void Airplane::moveDown() { speedDefault = 0.01f; }

void Airplane::onMouseMove(dvec2 mousePos) {
  dvec2 winSize = global::getWinSize();
  dvec2 winCenter = winSize * 0.5;
  dvec2 distFromCenter = mousePos - winCenter;

  vec2 delta = glm::radians(dvec2(camera.getSensitivity()) * distFromCenter / winCenter);
  vec3 camOrientation = camera.getOrientation();

  float cosAngle = dot(camera.getUp(), camOrientation);
  if (cosAngle * glm::sign(delta.y) > 0.99f)
    delta.y = 0.f;

  glm::quat q = glm::angleAxis(delta.x, camera.getUp());
  camOrientation = q * camOrientation;

  q = glm::angleAxis(delta.y, camera.getRight());
  camOrientation = q * camOrientation;

  camera.setOrientation(camOrientation);
  updateCamera();
}

void Airplane::onMouseScroll(dvec2 offset) {
  camDistance -= offset.y;
  camDistance = glm::clamp(camDistance, 1.f, 15.f);
}

Camera& Airplane::getCamera() {
  return camera;
}

void Airplane::setTurnSpeed(float speedRad) {
  turnSpeedRad = speedRad;
}

void Airplane::turn(float dir) {
  turnMomentumRad += turnSpeedRad * dir * global::dt;
  if (fabs(tiltRecoverMomentumRad) < PI_3)
    tiltMomentumRad += turnMomentumRad;
}

void Airplane::update(const Earth& earth) {
  trailLeft.update();
  trailRight.update();

  // Turn
  turnQuat = glm::angleAxis(turnMomentumRad, up);
  Mesh::rotate(turnQuat);
  vec3 right = turnQuat * getRight();
  vec3& forward = orientation;

  // Tilt
  Mesh::rotate(glm::angleAxis(tiltMomentumRad, -forward));
  tiltRecoverMomentumRad += tiltMomentumRad;

  // Move forward
  float frameSpeedRad = speed * global::dt;
  vec3 newPos = normalize(position) + forward * frameSpeedRad;
  vec3 gravityUp = normalize(newPos);
  newPos = gravityUp * (earth.getRadius() + flyHeight);
  Mesh::translate(newPos - position);

  // Rotate so that airplane's forward is perpendicular to the planet
  // Finding left since local left (-right) can be tilted up or down
  rotateQuat = glm::angleAxis(frameSpeedRad, normalize(cross(up, forward)));
  Mesh::rotate(rotateQuat);

  // Recovering from tilt
  float tiltRecover = tiltRecoverMomentumRad * tiltRecoverMomentumDecreaseFactor;
  Mesh::rotate(glm::angleAxis(tiltRecover, forward));
  tiltRecoverMomentumRad -= tiltRecover;

  up = gravityUp;
  right = -rotMat[0];
  forward = normalize(cross(up, right));
  position = newPos;
  turnMomentumRad *= turnMomentumDecreaseFactor;
  tiltMomentumRad *= tiltMomentumDecreaseFactor;

  // ========== Add left and right trails ========== //

  vec3 trailLeftPos = position;

  trailLeftPos += normalize(cross(forward, -right))  * trailOffset.y * meshScale;
  trailLeftPos += forward * trailOffset.z * meshScale;

  vec3 trailRightPos = trailLeftPos;

  trailLeftPos += right * trailOffset.x * meshScale;
  trailRightPos += -right * trailOffset.x * meshScale;

  trailLeft.add(trailLeftPos, trailDuration);
  trailRight.add(trailRightPos, trailDuration);

  // =============================================== //

  updateCamera();
}

void Airplane::draw(const Camera* cam, Shader& shader) const {
  Airplane::texDiffuse.bind();

  Mesh::draw(cam, shader);

  Airplane::texDiffuse.unbind();
}

void Airplane::drawTrail(const Camera* camera, Shader& shader) const {
  shader.setUniform1f("u_alphaFactor", trailAlphaFactor);

  trailLeft.draw(camera, shader);
  trailRight.draw(camera, shader);
}

void Airplane::drawDirections(const Camera* cam, Shader& shader) const {
  if (showRight)   meshes::line(position, position + getRight() , global::red).draw(cam, shader);
  if (showUp)      meshes::line(position, position + up         , global::green).draw(cam, shader);
  if (showForward) meshes::line(position, position + orientation, global::blue).draw(cam, shader);
}

void Airplane::updateCamera() {
  const vec3& pivot = position;
  vec3 actualBack = turnQuat * rotateQuat * camera.getBack();
  vec3 pos = pivot + actualBack * camDistance;

  camera.setUp(up);
  camera.setOrientation(-actualBack);
  camera.setPosition(pos);
  camera.update();
}

