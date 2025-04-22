#pragma once

#include "glm/ext/matrix_transform.hpp"

class Camera {
public:
  Camera(vec3 pos, vec3 orientation, float sensitivity);

  const float& getLongitude() const;
  const float& getLatitude() const;
  const vec3& getPosition() const;
  const mat4& getMatrix() const;
  const vec3& getUp() const;

  vec3 getViewDir() const;
  vec3 getRight() const;

  void setIncreasedSpeed();
  void setNormalSpeed();
  void setLongitude(float l);
  void setLatitude(float l);

  void update();
  void moveForward();
  void moveBack();
  void moveLeft();
  void moveRight();
  void moveUp();
  void moveDown();
  void moveByMouse(const double& x, const double& y);

private:
  vec3 position, orientation;
  float sensitivity;

  vec3 up{0.f, 1.f, 0.f};
  float lon = 0.f;
  float lat = 0.f;
  float speed = 3.f;
  float fov = 45.f;

  float prevLon = lon;
  float prevLat = lat;

  mat4 mat = identity<mat4>();
  mat4 view = identity<mat4>();

private:
  void calcView();
  void calcOrientation(const float& radRotX, const float& radRotY);
};

