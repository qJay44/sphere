#include "global.hpp"

namespace global {

GLFWwindow* window = nullptr;

float dt = 0.f;

bool guiFocused     = false;
bool camIsArcball   = true;
bool drawWireframe  = false;
bool drawNormals    = false;
bool drawGlobalAxis = false;

}// global

