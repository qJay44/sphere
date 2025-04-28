#pragma once

#include "Camera.hpp"
#include "Airplane.hpp"

class AirplaneCamera : public Camera {
public:
  AirplaneCamera(const Airplane& airplane, float distance, float sensitivity);

  void moveByMouse(const double& x, const double& y) override;

  void moveForward() override;
  void moveBack()    override;
  void moveLeft()    override;
  void moveRight()   override;
  void moveUp()      override;
  void moveDown()    override;

private:
  const Airplane& airplane;
  float distance;

private:
  void calcView() override;
};

