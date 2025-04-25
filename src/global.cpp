#include "global.hpp"

namespace global {

float nearPlane = 0.1f;
float farPlane  = 100.f;

int winWidth  = 1200;
int winHeight = 720;

float dt = 0.f;

float planetRadius = 1.f;
float orbitRadius  = planetRadius * 4.f;
float orbitSpeed   = 1.f; // radians

bool guiFocused     = false;
bool camIsArcball   = false;
bool drawWireframe  = false;
bool drawNormals    = false;

}// global

