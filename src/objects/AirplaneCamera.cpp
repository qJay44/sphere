#include "AirplaneCamera.hpp"

#include "Airplane.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "../engine/CameraStorage.hpp"

AirplaneCamera* CameraStorage::cameraAirplanePtr = nullptr;

AirplaneCamera::AirplaneCamera(Airplane& airplane, float distance, float sensitivity)
  : Camera(airplane.getPosition() + airplane.getUp() * distance, vec3(0.f, 0.f, -1.f), sensitivity),
    airplane(airplane),
    distance(distance) {
  farPlane = 50.f;

  // Adjust view direction so it is not exactly above the airplane
  glm::quat quat = glm::angleAxis(PI / 10.f, getRight());
  position = quat * (position - airplane.getPosition()) + airplane.getPosition();
  calcView();
}

const float& AirplaneCamera::getDistance() const { return distance; }

void AirplaneCamera::setDistance(const float& d) { distance = d; }

void AirplaneCamera::moveForward() {}
void AirplaneCamera::moveBack()    {}
void AirplaneCamera::moveLeft()    {airplane.turn( 1.f);}
void AirplaneCamera::moveRight()   {airplane.turn(-1.f);}
void AirplaneCamera::moveUp()      {}
void AirplaneCamera::moveDown()    {}

void AirplaneCamera::moveByMouse(const dvec2& mousePos) {
  // Window size and center
  ivec2 winSize;
  glfwGetWindowSize(global::window, &winSize.x, &winSize.y);
  dvec2 winCenter = dvec2(winSize) / 2.;

  const vec3& pivot = airplane.getPosition();
  vec2 radRot = glm::radians(sensitivity * (mousePos - winCenter) / winCenter);
  vec3 pos(airplane.getPosition() + getBack() * distance);

  // Prevent view direction to be exactly above the airplane
  float cosAngle = dot(up, getForward());
  if (cosAngle * glm::sign(radRot.y) > 0.99f)
    radRot.y = 0.f;

  // Rotate horizontally
  glm::quat quat = glm::angleAxis(radRot.x, up);
  pos = quat * (pos - pivot) + pivot;

  // Rotate vertically
  quat = glm::angleAxis(radRot.y, getRight());
  position = quat * (pos - pivot) + pivot;
}

void AirplaneCamera::calcView() {
  view = glm::lookAt(position, airplane.getPosition(), up);
  up = airplane.getUp();
}

void AirplaneCamera::zoom(const float& dir) {
  distance -= dir;
  if (distance < 1.f)
    distance = 1.f;
  else
    position += getForward() * dir;
}

