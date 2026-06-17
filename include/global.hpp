#pragma once

#include "ProfilerManager.hpp"

namespace global {

static const union { vec3 right   {1.f, 0.f, 0.f}, red;   };
static const union { vec3 up      {0.f, 1.f, 0.f}, green; };
static const union { vec3 forward {0.f, 0.f, 1.f}, blue;  };

extern GLFWwindow* window;
extern ProfilerManager* profiler;

extern float dt;
extern float time;
extern float gamma;

extern bool controllingAirplane;
extern bool guiFocused;
extern bool wireframeMode;
extern bool drawGlobalAxis;

static inline ivec2 getWinSize() {
  ivec2 res;
  glfwGetWindowSize(global::window, &res.x, &res.y);
  return res;
}

static inline dvec2 getWinCenter() {
  return dvec2(getWinSize()) * 0.5;
}

static inline dvec2 getMousePos() {
  dvec2 res;
  glfwGetCursorPos(global::window, &res.x, &res.y);
  return res;
}

} // namespace global

