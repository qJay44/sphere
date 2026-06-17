#include "global.hpp"

namespace global {

GLFWwindow* window = nullptr;
ProfilerManager* profiler = nullptr;

float dt = 0.f;
float time = 0.f;
float gamma = 2.2f;

bool controllingAirplane = true;
bool guiFocused     = false;
bool wireframeMode  = false;
bool drawGlobalAxis = false;

}// global

