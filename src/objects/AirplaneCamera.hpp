#pragma once

#include "../engine/Camera.hpp"
#include "Airplane.hpp"

class AirplaneCamera : public Camera {
public:
  AirplaneCamera(const Airplane& airplane, float distance, float sensitivity);

  void moveForward() override;
  void moveBack()    override;
  void moveLeft()    override;
  void moveRight()   override;
  void moveUp()      override;
  void moveDown()    override;

  void moveByMouse(const double& x, const double& y) override;

  void zoom(const float& dir);

private:
  const Airplane& airplane;
  float distance;

  mat4 rot;

private:
  void calcView() override;
};

