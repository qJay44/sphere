#include "global.hpp"

namespace global {

float nearPlane = 0.1f;
float farPlane  = 100.f;

int winWidth  = 1200;
int winHeight = 720;

float dt = 0.f;

bool guiFocused     = false;
bool camIsArcball   = true;
bool drawWireframe  = false;
bool drawNormals    = false;
bool drawDirections = true;

}// global

