#include "ArcballCamera.hpp"

ArcballCamera::ArcballCamera(vec3 pos, vec3 orientation, float sensitivity)
  : Camera(pos, orientation, sensitivity) {}

void ArcballCamera::moveForward() { lat += global::orbitSpeed * global::dt; }
void ArcballCamera::moveBack()    { lat -= global::orbitSpeed * global::dt; }
void ArcballCamera::moveLeft()    { lon -= global::orbitSpeed * global::dt; }
void ArcballCamera::moveRight()   { lon += global::orbitSpeed * global::dt; }

const float& ArcballCamera::getLongitude() const { return lon; }
const float& ArcballCamera::getLatitude()  const { return lat; }

void ArcballCamera::setLatitude(float l)  { lat = l; update(); }
void ArcballCamera::setLongitude(float l) { lon = l; update(); }

void ArcballCamera::calcView() {
  static const vec4 pivot(vec3(0.f), 1.f); // Always looking at the center
  vec4 pos(position, 1.f);

  float dx = lon - prevLon;
  float dy = lat - prevLat;

  if (lon > PI)       lon = -PI;
  else if (lon < -PI) lon = PI;

  if (lat > PI_2)       lat = -PI_2;
  else if (lat < -PI_2) lat = PI_2;

  // FIXME: Vertical lock
  float cosAngle = dot(getViewDir(), up);
  if ((cosAngle * (abs(dy) / dy)) > 0.99f)
    dy = 0.f;

  mat4 rotX(1.f);
  rotX = rotate(rotX, dx, up);
  pos = (rotX * (pos - pivot)) + pivot;

  mat4 rotY(1.f);
  rotY = rotate(rotY, dy, getRight());
  position = (rotY * (pos - pivot)) + pivot;

  view = lookAt(position, vec3(0.f), up);

  prevLon = lon;
  prevLat = lat;
}

