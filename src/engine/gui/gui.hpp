#pragma once

#include "../objects/Airplane.hpp"
#include "../objects/Earth.hpp"
#include "../objects/Sun.hpp"

struct gui {
  static Earth* earthPtr;
  static Camera* camSpecatePtr;
  static Airplane* airplanePtr;
  static Sun* sunPtr;
  static u16 fps;

  static float _slider1f0;
  static float _slider1f1;
  static float _slider1f2;
  static vec2 _slider2f0;
  static vec2 _slider2f1;
  static vec2 _slider2f2;
  static vec3 _slider3f0;
  static vec3 _slider3f1;
  static vec3 _slider3f2;
  static int _slider1i0;
  static int _slider1i1;
  static int _slider1i2;

  static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

  static void init();
  static void toggleConfig();
  static void toggleInfo();
  static void draw();
  static void shutdown();
};

