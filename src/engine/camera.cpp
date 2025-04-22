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

const float& Camera::getLongitude() const { return lon; }
const float& Camera::getLatitude()  const { return lat; }
const vec3& Camera::getPosition()   const { return position; }
const mat4& Camera::getMatrix()     const { return mat; }
const vec3& Camera::getUp()         const { return up; }

vec3 Camera::getViewDir() const { return  transpose(view)[2]; }
vec3 Camera::getRight()   const { return -transpose(view)[0]; }

void Camera::setIncreasedSpeed()   { speed = 8.f; }
void Camera::setNormalSpeed()      { speed = 3.f; }
void Camera::setLatitude(float l)  { lat = l; update(); }
void Camera::setLongitude(float l) { lon = l; update(); }

void Camera::update() {
  calcView();
  float aspectRatio = (float)_gcfg.winWidth / _gcfg.winHeight;
  mat4 proj = perspective(radians(fov), aspectRatio, _gcfg.nearPlane, _gcfg.farPlane);

  mat = proj * view;
}

void Camera::moveForward() { lat += _gcfg.orbitSpeed * _gcfg.dt; }
void Camera::moveBack()    { lat -= _gcfg.orbitSpeed * _gcfg.dt; }
void Camera::moveLeft()    { lon -= _gcfg.orbitSpeed * _gcfg.dt; }
void Camera::moveRight()   { lon += _gcfg.orbitSpeed * _gcfg.dt; }

void Camera::moveUp()   { position +=  up * speed; }
void Camera::moveDown() { position += -up * speed; }

void Camera::moveByMouse(const double& x, const double& y) {
  // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
  // and then "transforms" them into degrees
  double rotX = sensitivity * (y - _gcfg.winHeight * 0.5f) / _gcfg.winHeight;
  double rotY = sensitivity * (x - _gcfg.winWidth * 0.5f) / _gcfg.winWidth;

  float radRotX = static_cast<float>(radians(-rotX));
  float radRotY = static_cast<float>(radians(-rotY));

  calcOrientation(radRotX, radRotY);
}

void Camera::calcView() {
  static const vec4 pivot(vec3(0.f), 1.f); // Always looking at the center
  vec4 pos(position, 1.f);

  float diffLon = lon - prevLon;
  float diffLat = lat - prevLat;

  if (lon > PI)       lon = -PI;
  else if (lon < -PI) lon = PI;

  if (lat > PI_2)       lat = -PI_2;
  else if (lat < -PI_2) lat = PI_2;

  // FIXME: Vertical lock
  float cosAngle = dot(getViewDir(), up);
  if ((cosAngle * (abs(diffLat) / diffLat)) > 0.99f)
    diffLat = 0.f;

  mat4 rotX(1.f);
  rotX = rotate(rotX, diffLon, up);
  pos = (rotX * (pos - pivot)) + pivot;

  mat4 rotY(1.f);
  rotY = rotate(rotY, diffLat, getRight());
  position = (rotY * (pos - pivot)) + pivot;

  view = lookAt(position, vec3(0.f), up);

  prevLon = lon;
  prevLat = lat;
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

