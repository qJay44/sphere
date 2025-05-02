#include "AirplaneCamera.hpp"

#include "Airplane.hpp"
#include "glm/ext/matrix_transform.hpp"

AirplaneCamera::AirplaneCamera(const Airplane& airplane, float distance, float sensitivity)
  : Camera(airplane.getPosition() + airplane.getUp() * distance, vec3(0.f, 0.f, -1.f), sensitivity),
    airplane(airplane),
    distance(distance) {}

void AirplaneCamera::moveForward() {}
void AirplaneCamera::moveBack()    {}
void AirplaneCamera::moveLeft()    {}
void AirplaneCamera::moveRight()   {}
void AirplaneCamera::moveUp()      {}
void AirplaneCamera::moveDown()    {}

void AirplaneCamera::moveByMouse(const dvec2& mousePos) {
  ivec2 winSize;
  glfwGetWindowSize(global::window, &winSize.x, &winSize.y);
  dvec2 winCenter = winSize / 2;

  vec2 radRot = glm::radians(sensitivity * (mousePos - winCenter) / winCenter);
  vec4 pivot(airplane.getPosition(), 1.f);
  vec4 pos(airplane.getPosition() + getBack() * distance, 1.f);

  float cosAngle = dot(getBack(), up);
  if ((cosAngle * (abs(radRot.y) / radRot.y)) > 0.99f)
    radRot.y = 0.f;

  mat4 matRotX(1.f);
  matRotX = glm::rotate(matRotX, radRot.x, up);
  pos = (matRotX * (pos - pivot)) + pivot;

  mat4 matRotY(1.f);
  matRotY = glm::rotate(matRotY, radRot.y, getLeft());
  position = (matRotY * (pos - pivot)) + pivot;
}

void AirplaneCamera::calcView() {
  view = glm::lookAt(position, airplane.getPosition(), up);
}

void AirplaneCamera::zoom(const float& dir) {
  distance -= dir;
  if (distance < 1.f)
    distance = 1.f;
  else
    position += getForward() * dir;
}

