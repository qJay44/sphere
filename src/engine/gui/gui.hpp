#pragma once

#include "../objects/Airplane.hpp"
#include "../objects/Earth.hpp"
#include "../objects/Light.hpp"

struct gui {
  static Earth* earthPtr;
  static Camera* camSpecatePtr;
  static Airplane* airplanePtr;
  static Light* lightPtr;
  static u16 fps;

  static float _sliderf0;
  static int _slideri0;

  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

  static void init();
  static void toggle();
  static void draw();
  static void shutdown();
};

