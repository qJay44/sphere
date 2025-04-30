#include "Camera.hpp"

#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/trigonometric.hpp"

Camera::Camera(vec3 pos, vec3 orientation, float sensitivity)
  : position(pos),
    orientation(orientation),
    sensitivity(sensitivity) {
  calcView();
};

const vec3& Camera::getOrientation() const { return orientation; }
const vec3& Camera::getPosition()    const { return position; }
const mat4& Camera::getMatrix()      const { return mat; }
const vec3& Camera::getUp()          const { return up; }

vec3 Camera::getBack()  const { return  transpose(view)[2]; }
vec3 Camera::getRight() const { return -transpose(view)[0]; }

void Camera::setIncreasedSpeed()   { speed = 12.f * global::dt; }
void Camera::setNormalSpeed()      { speed = 3.f  * global::dt; }

void Camera::update() {
  float aspectRatio = static_cast<float>(global::winWidth) / global::winHeight;
  mat4 proj = perspective(radians(fov), aspectRatio, global::nearPlane, global::farPlane);

  calcView();
  mat = proj * view;
}

void Camera::moveForward() { position +=  orientation * speed; }
void Camera::moveBack()    { position += -orientation * speed; }
void Camera::moveLeft()    { position += -normalize(cross(orientation, up)) * speed; }
void Camera::moveRight()   { position +=  normalize(cross(orientation, up)) * speed; }

void Camera::moveUp()   { position +=  up * speed; }
void Camera::moveDown() { position += -up * speed; }

void Camera::moveByMouse(const double& x, const double& y) {
  // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
  // and then "transforms" them into degrees
  double rotX = sensitivity * (y - global::winHeight * 0.5f) / global::winHeight;
  double rotY = sensitivity * (x - global::winWidth * 0.5f) / global::winWidth;

  float radRotX = static_cast<float>(radians(-rotX));
  float radRotY = static_cast<float>(radians(-rotY));

  calcOrientation(radRotX, radRotY);
}

void Camera::calcView() {
  view = lookAt(position, position + orientation, up);
}

void Camera::calcOrientation(const float& radRotX, const float& radRotY) {
  // Calculates upcoming vertical change in the Orientation
  vec3 newOrientation = rotate(orientation, radRotX, normalize(cross(orientation, up)));

  // Decides whether the next vertical Orientation is legal or not
  if (!(angle(newOrientation, up) <= radians(5.f) || angle(newOrientation, -up) <= radians(5.f)))
    orientation = newOrientation;

  // Rotates the Orientation left and right
  orientation = rotate(orientation, radRotY, up);
}

