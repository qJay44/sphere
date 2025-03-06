#include "Camera.hpp"

#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

Camera::Camera(vec3 pos, vec3 orientation, float sensitivity)
  : position(pos),
    orientation(orientation),
    sensitivity(sensitivity) {
  calcView();
};

const vec2& Camera::getLookCoords() const { return lookCoords; }
const vec3& Camera::getPosition() const { return position; }
const mat4& Camera::getMatrix() const { return mat; }

void Camera::calcView() {
  if (lookCoords.x > PI) lookCoords.x = -PI;
  else if (lookCoords.x < -PI) lookCoords.x = PI;

  /* if (lookCoords.y > PI_2) lookCoords.y = -PI_2; */
  /* else if (lookCoords.y < -PI_2) lookCoords.y = PI_2; */

  float x = _gcfg.orbitRadius * sin(lookCoords.x) * cos(lookCoords.y);
  float y = _gcfg.orbitRadius * sin(lookCoords.y);
  float z = _gcfg.orbitRadius * cos(lookCoords.x) * cos(lookCoords.y);

  position = {x, y, z};
  view = lookAt(position + orientation, vec3(0.f), up);
}

void Camera::update() {
  calcView();
  float aspectRatio = (float)_gcfg.winWidth / _gcfg.winHeight;
  mat4 proj = perspective(radians(fov), aspectRatio, _gcfg.nearPlane, _gcfg.farPlane);

  mat = proj * view;
}

void Camera::moveForward() { lookCoords.y += _gcfg.orbitSpeed * _gcfg.dt; }
void Camera::moveBack() { lookCoords.y -= _gcfg.orbitSpeed * _gcfg.dt; }
void Camera::moveLeft() { lookCoords.x -= _gcfg.orbitSpeed * _gcfg.dt; }
void Camera::moveRight() { lookCoords.x += _gcfg.orbitSpeed * _gcfg.dt; }

void Camera::moveUp() { position += up * speed; }
void Camera::moveDown() { position += up * -speed; }

void Camera::setIncreasedSpeed() { speed = 8.f; }
void Camera::setNormalSpeed() { speed = 3.f; }

void Camera::setLookCoords(vec2 v) {
  lookCoords = v;
  update();
}

void Camera::move(double x, double y) {
  // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
  // and then "transforms" them into degrees
  double rotX = sensitivity * (y - _gcfg.winHeight * 0.5f) / _gcfg.winHeight;
  double rotY = sensitivity * (x - _gcfg.winWidth * 0.5f) / _gcfg.winWidth;
  float radRotX = static_cast<float>(radians(-rotX));
  float radRotY = static_cast<float>(radians(-rotY));

  // Calculates upcoming vertical change in the Orientation
  vec3 newOrientation = rotate(orientation, radRotX, normalize(cross(orientation, up)));

  // Decides whether the next vertical Orientation is legal or not
  if (!(angle(newOrientation, up) <= radians(5.f) || angle(newOrientation, -up) <= radians(5.f)))
    orientation = newOrientation;

  // Rotates the Orientation left and right
  orientation = rotate(orientation, radRotY, up);
}
