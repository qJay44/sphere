#include "global.hpp"

namespace global {

GLFWwindow* window = nullptr;

float nearPlane = 0.1f;
float farPlane  = 100.f;

float dt = 0.f;

bool guiFocused     = false;
bool camIsArcball   = true;
bool drawWireframe  = false;
bool drawNormals    = false;
bool drawDirections = true;

}// global

