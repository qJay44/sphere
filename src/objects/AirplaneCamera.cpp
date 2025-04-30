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

void AirplaneCamera::moveByMouse(const double& x, const double& y) {
  static vec3 apPrev = airplane.getPosition();
  const vec3& apCurr = airplane.getPosition();
  vec3 diff = apCurr - apPrev;

  float radRotX = glm::radians(sensitivity * (x - global::winWidth * 0.5f) / global::winWidth);
  float radRotY = glm::radians(sensitivity * (y - global::winHeight * 0.5f) / global::winHeight);

  vec4 pivot(apCurr, 1.f);
  vec4 pos(position + diff, 1.f);

  float cosAngle = dot(getBack(), up);
  if ((cosAngle * (abs(radRotY) / radRotY)) > 0.99f)
    radRotY = 0.f;

  mat4 matRotX(1.f);
  matRotX = glm::rotate(matRotX, radRotX, up);
  pos = (matRotX * (pos - pivot)) + pivot;

  mat4 matRotY(1.f);
  matRotY = glm::rotate(matRotY, radRotY, getLeft());
  position = (matRotY * (pos - pivot)) + pivot;
  apPrev = apCurr;
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

