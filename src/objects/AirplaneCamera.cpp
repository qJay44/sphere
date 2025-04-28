#include "AirplaneCamera.hpp"

#include "Airplane.hpp"

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

  double rotX = sensitivity * (x - global::winWidth * 0.5f) / global::winWidth;
  double rotY = sensitivity * (y - global::winHeight * 0.5f) / global::winHeight;

  float radRotX = static_cast<float>(radians(rotX));
  float radRotY = static_cast<float>(radians(rotY));

  vec4 pivot(apCurr, 1.f);
  vec4 pos(position + diff, 1.f);

  float cosAngle = dot(getViewDir(), up);
  if ((cosAngle * (abs(radRotY) / radRotY)) > 0.99f)
    radRotY = 0.f;

  mat4 matRotX(1.f);
  matRotX = rotate(matRotX, radRotX, up);
  pos = (matRotX * (pos - pivot)) + pivot;

  mat4 matRotY(1.f);
  matRotY = rotate(matRotY, radRotY, getRight());
  position = (matRotY * (pos - pivot)) + pivot;
  apPrev = apCurr;
}

void AirplaneCamera::calcView() {
  view = lookAt(position, airplane.getPosition(), up);
}

