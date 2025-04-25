#pragma once

#include "Camera.hpp"

class ArcballCamera : public Camera {
public:
  ArcballCamera(vec3 pos, vec3 orientation, float sensitivity);

  const float& getLongitude() const;
  const float& getLatitude()  const;

  void setLongitude(float l);
  void setLatitude(float l);

  void moveForward() override;
  void moveBack()    override;
  void moveLeft()    override;
  void moveRight()   override;

private:
  float lon = 0.f;
  float lat = 0.f;

  float prevLon = lon;
  float prevLat = lat;

private:
  void calcView() override;
};

