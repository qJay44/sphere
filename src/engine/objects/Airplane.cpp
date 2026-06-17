#include "Airplane.hpp"

#include "glm/common.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/quaternion.hpp"
#include "global.hpp"

Texture2D Airplane::texDiffuse;

Airplane::Airplane(vec3 position, float flyHeight, const fspath& model, float meshScale)
  : Moveable(position, 0.f, 0.f),
    Mesh(Mesh::loadObj(model)),
    camera(position),
    flyHeight(flyHeight),
    meshScale(meshScale),
    lightLeft(position, 0.1f, global::green),
    lightRight(position, 0.1f, global::red)
{
  if (texDiffuse.getId() == 0)
    texDiffuse = Texture2D(image2D("res/tex/airplane/11804_Airplane_diff.jpg", IMAGE2D_LOAD_STB, true), {});

  // Facing +Y
  Mesh::scale(meshScale);
  Moveable::setOrientation(localOrientation);
  auto q1 = glm::angleAxis(PI, localUp);
  auto q2 = glm::angleAxis(PI_2, localRight);
  localRotationQuat = q1 * q2;
  up = localUp;

  camera.setPosition(position + getBack() * camDistance);
  camera.setView(this);
  camera.update();

  trailLeft.width = 0.02f;
  trailLeft.duration = 0.5f;
}

void Airplane::moveForward() {}
void Airplane::moveBack()    {}

void Airplane::moveLeft()  { turn(-1.f); }
void Airplane::moveRight() { turn( 1.f); }

void Airplane::moveUp()   { stop = true;  }
void Airplane::moveDown() { stop = false; }

void Airplane::onMouseMove(dvec2 mousePos) {
  camera.onMouseMove(mousePos);
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

void Airplane::setFlags(u32 f) {
  flags = f;
}

void Airplane::turn(float dir) {
  turnMomentumRad += turnSpeedRad * dir * global::dt;
  tiltMomentumRad += turnMomentumRad;
}

void Airplane::update(const Earth& earth) {
  float height = earth.getRadius() + flyHeight;
  float frameSpeedRad = -(speed * global::dt) / height;

  yawQuat = glm::angleAxis(turnMomentumRad, -localUp);
  rollQuat = glm::angleAxis(tiltMomentumRad, localOrientation);
  pitchQuat = glm::angleAxis(frameSpeedRad, localRight);

  if (!stop)
    orientationQuat = orientationQuat * yawQuat * pitchQuat;

  up = orientationQuat * localUp;
  orientation = orientationQuat * localOrientation;
  position = up * height;

  rotMat = glm::mat4_cast(orientationQuat * rollQuat * localRotationQuat);
  Mesh::setTrans(position);

  turnMomentumRad *= turnMomentumDamping;
  tiltMomentumRad *= tiltMomentumDamping;

  updateCamera();
  updateTrails();
}

void Airplane::draw(const Camera* camera, Shader& shader) const {
  Airplane::texDiffuse.bind(0);
  Mesh::draw(camera, shader);
}

void Airplane::drawTrails(const Camera* camera, Shader& shader) const {
  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  trailLeft.draw(camera, shader);
  trailRight.draw(camera, shader);

  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
}

void Airplane::drawLights(const Camera* camera, Shader& shader) const {
  lightLeft.draw(camera, shader);
  lightRight.draw(camera, shader);
}

void Airplane::drawDirections(const Camera* camera, Shader& shader) const {
  auto p = position;
  if (flags & AirplaneFlags_DrawRight)   Mesh::drawDirectionLine(camera, shader, p, getRight() , global::red);
  if (flags & AirplaneFlags_DrawUp)      Mesh::drawDirectionLine(camera, shader, p, up         , global::green);
  if (flags & AirplaneFlags_DrawForward) Mesh::drawDirectionLine(camera, shader, p, orientation, global::blue);
}

void Airplane::updateCamera() {
  glm::quat customYaw = glm::angleAxis(camera.getYaw(), localUp);
  glm::quat customPitch = glm::angleAxis(camera.getPitch(), localRight);
  glm::quat camOrientationQuat = orientationQuat * customYaw * customPitch;

  vec3 camForward = camOrientationQuat * localOrientation;
  vec3 camUp = camOrientationQuat * localUp;
  vec3 camBack = -camForward;

  vec3 pos = position + camBack * camDistance;

  camera.setUp(camUp);
  camera.setOrientation(camForward);
  camera.setPosition(pos);
  camera.update();
}

void Airplane::updateTrails() {
  const vec3& right = orientationQuat * rollQuat * localRight;

  vec3 trailLeftPos = position;

  trailLeftPos += normalize(cross(orientation, -right))  * trailOffset.y * meshScale;
  trailLeftPos += orientation * trailOffset.z * meshScale;

  vec3 trailRightPos = trailLeftPos;

  trailLeftPos += right * trailOffset.x * meshScale;
  trailRightPos += -right * trailOffset.x * meshScale;

  trailLeft.add(trailLeftPos);
  trailRight.add(trailRightPos);

  lightLeft.setTrans(trailLeftPos);
  lightRight.setTrans(trailRightPos);

  trailRight.width = trailLeft.width;
  trailRight.duration = trailLeft.duration;

  trailLeft.update(&camera);
  trailRight.update(&camera);
}

