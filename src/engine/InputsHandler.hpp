#pragma once

#include "../objects/AirplaneCamera.hpp"

struct InputsHandler {
  InputsHandler() = delete;

  static AirplaneCamera* airplaneCameraPtr;

  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

  static void process(Camera* camera);
};
