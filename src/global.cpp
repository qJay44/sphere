#include "global.hpp"

namespace global {

GLFWwindow* window = nullptr;

float dt = 0.f;

bool guiFocused     = false;
bool camIsArcball   = false;
bool drawWireframe  = false;
bool drawNormals    = false;
bool drawDirections = true;

}// global

