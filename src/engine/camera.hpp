#pragma once

class Camera {
public:
  Camera(vec3 pos, vec3 orientation, float sensitivity);

  const vec3& getPosition() const;
  const mat4& getMatrix() const;

  void update(double dt);
  void moveForward();
  void moveBack();
  void moveLeft();
  void moveRight();
  void moveUp();
  void moveDown();
  void setIncreasedSpeed();
  void setNormalSpeed();
  void move(double x, double y);

private:
  vec3 position, orientation, up;
  float speed, sensitivity, fov;
  mat4 mat;
};

