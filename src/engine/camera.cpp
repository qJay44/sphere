#include "Camera.hpp"

#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

Camera::Camera(vec3 pos, vec3 orientation, float sensitivity)
  : position(pos),
    orientation(orientation),
    up({0.f, 1.f, 0.f}),
    speed(100.f),
    sensitivity(sensitivity),
    fov(45.f) {};

const vec3& Camera::getPosition() const { return position; }

const mat4& Camera::getMatrix() const { return mat; }

void Camera::update(double dt) {
  mat4 view = identity<mat4>();
  mat4 proj = identity<mat4>();

  float aspectRatio = (float)_gcfg.winWidth / _gcfg.winHeight;
  speed *= static_cast<float>(dt);

  vec3 lookPos = position + orientation;

  view = lookAt(position, lookPos, up);
  proj = perspective(radians(fov), aspectRatio, _gcfg.nearPlane, _gcfg.farPlane);

  mat = proj * view;
}

void Camera::moveForward() {
  vec3 quotient = orientation * speed;
  position = position + quotient;
}

void Camera::moveBack() {
  vec3 quotient = orientation * -speed;
  position = position + quotient;
}

void Camera::moveLeft() {
  vec3 normCross = normalize(cross(orientation, up));
  vec3 quotient = normCross * -speed;
  position = position + quotient;
}

void Camera::moveRight() {
  vec3 normCross = normalize(cross(orientation, up));
  vec3 quotient = normCross * speed;
  position = position + quotient;
}

void Camera::moveUp() {
  vec3 quotient = up * speed;
  position = position + quotient;
}

void Camera::moveDown() {
  vec3 quotient = up * -speed;
  position = position + quotient;
}

void Camera::setIncreasedSpeed() { speed = 4.f; }

void Camera::setNormalSpeed() { speed = 3.f; }

void Camera::move(double x, double y) {
  // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
  // and then "transforms" them into degrees
  double rotX = sensitivity * (y - _gcfg.winHeight * 0.5f) / _gcfg.winHeight;
  double rotY = sensitivity * (x - _gcfg.winWidth * 0.5f) / _gcfg.winWidth;
  float radRotX = static_cast<float>(radians(-rotX));
  float radRotY = static_cast<float>(radians(-rotY));

  // Calculates upcoming vertical change in the Orientation
  vec3 newOrientation = rotate(orientation, radRotX, normalize(cross(orientation, up)));
  vec3 upOpposite = up * -1.f;

  // Decides whether or not the next vertical Orientation is legal or not
  if (!(angle(newOrientation, up) <= radians(5.f) || angle(newOrientation, upOpposite) <= radians(5.f)))
    orientation = newOrientation;

  // Rotates the Orientation left and right
  orientation = rotate(orientation, radRotY, up);
}
