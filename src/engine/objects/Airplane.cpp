#include "Airplane.hpp"

#include "../mesh/meshes.hpp"
#include "glm/common.hpp"

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
  if (texDiffuse.getUniformName().empty())
    texDiffuse = Texture2D(image2D("res/tex/airplane/11804_Airplane_diff.jpg", IMAGE2D_LOAD_STB, true), {"diffuse0", 0});

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

  trailLeft.width = 0.02f;
  trailLeft.duration = 0.5f;
}

Airplane::~Airplane() {
  Mesh::clear();
}

void Airplane::moveForward() {}
void Airplane::moveBack()    {}

void Airplane::moveLeft()  { turn( 1.f); }
void Airplane::moveRight() { turn(-1.f); }

void Airplane::moveUp()   { speedDefault = 0.f;   }
void Airplane::moveDown() { speedDefault = 0.05f; }

void Airplane::onMouseMove(dvec2 mousePos) {
  dvec2 winSize = global::getWinSize();
  dvec2 winCenter = winSize * 0.5;
  dvec2 distFromCenter = mousePos - winCenter;

  vec2 delta = glm::radians(dvec2(camera.getSensitivity()) * distFromCenter / winCenter);
  vec3 camOrientation = camera.getOrientation();

  // No vertical rotation if almost looking down or up
  float cosAngle = dot(camera.getUp(), camOrientation);
  if (cosAngle * glm::sign(delta.y) > 0.99f)
    delta.y = 0.f;

  // Horizontal
  glm::quat q = glm::angleAxis(delta.x, camera.getUp());
  camOrientation = q * camOrientation;

  // Vertical
  q = glm::angleAxis(delta.y, camera.getRight());
  camOrientation = q * camOrientation;

  camera.setOrientation(camOrientation);
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
  // Turn
  turnQuat = glm::angleAxis(turnMomentumRad, up);
  Mesh::rotate(turnQuat);
  vec3 right = turnQuat * getRight();

  // Tilt
  Mesh::rotate(glm::angleAxis(tiltMomentumRad, -orientation));
  tiltRecoverMomentumRad += tiltMomentumRad;

  // Move forward
  float dtSafe = glm::min(global::dt, 0.033f);
  float frameSpeedRad = speed * dtSafe;
  vec3 newPos = normalize(position) + orientation * frameSpeedRad;
  vec3 gravityUp = normalize(newPos);
  newPos = gravityUp * (earth.getRadius() + flyHeight);
  Mesh::translate(newPos - position);

  // Rotate so that airplane's orientation is perpendicular to the planet
  // Finding a new left since local left (-right) can be tilted up or down
  rotateQuat = glm::angleAxis(frameSpeedRad, normalize(cross(up, orientation)));
  Mesh::rotate(rotateQuat);

  // Recovering from tilt
  float tiltRecover = tiltRecoverMomentumRad * tiltRecoverMomentumDecreaseFactor;
  Mesh::rotate(glm::angleAxis(tiltRecover, orientation));
  tiltRecoverMomentumRad -= tiltRecover;

  up = gravityUp;
  right = -rotMat[0];
  orientation = normalize(cross(up, right));
  position = newPos;
  turnMomentumRad *= turnMomentumDecreaseFactor;
  tiltMomentumRad *= tiltMomentumDecreaseFactor;

  updateCamera();
  updateTrails();
}

void Airplane::draw(const Camera* camera, Shader& shader) const {
  Airplane::texDiffuse.bind();

  Mesh::draw(camera, shader);

  Airplane::texDiffuse.unbind();
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
  if (flags & AirplaneFlags_DrawRight)   meshes::line(position, position + getRight() , global::red  ).draw(camera, shader);
  if (flags & AirplaneFlags_DrawUp)      meshes::line(position, position + up         , global::green).draw(camera, shader);
  if (flags & AirplaneFlags_DrawForward) meshes::line(position, position + orientation, global::blue ).draw(camera, shader);
}

void Airplane::updateCamera() {
  vec3 actualBack = turnQuat * rotateQuat * camera.getBack();
  vec3 pos = position + actualBack * camDistance;

  camera.setUp(up);
  camera.setOrientation(-actualBack);
  camera.setPosition(pos);
  camera.update();
}

void Airplane::updateTrails() {
  const vec3& right = -rotMat[0];

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

