#pragma once

#include "glm/ext/matrix_transform.hpp"

class Camera {
public:
  Camera(vec3 pos, vec3 orientation, float sensitivity);

  const vec2& getLookCoords() const;
  const vec3& getPosition() const;
  const mat4& getMatrix() const;

  void calcView();
  void update();
  void moveForward();
  void moveBack();
  void moveLeft();
  void moveRight();
  void moveUp();
  void moveDown();
  void move(double x, double y);

  void setIncreasedSpeed();
  void setNormalSpeed();
  void setLookCoords(vec2 v);

private:
  vec3 position, orientation;
  float sensitivity;

  vec3 up{0.f, 1.f, 0.f};
  vec2 lookCoords{0.1f, 0.1f};
  float speed = 3.f;
  float fov = 45.f;

  mat4 mat = identity<mat4>();
  mat4 view = identity<mat4>();
};

