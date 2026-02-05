#include "global.hpp"

namespace global {

GLFWwindow* window = nullptr;

float dt = 0.f;
float time = 0.f;

bool controllingAirplane = true;
bool guiFocused     = false;
bool drawWireframe  = false;
bool drawNormals    = false;
bool drawGlobalAxis = false;

}// global

