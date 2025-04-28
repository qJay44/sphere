#pragma once

#include "objects/Planet.hpp"
#include "objects/AirplaneCamera.hpp"

namespace gui {

void link(Planet* ptr);
void link(AirplaneCamera* ptr);
void link(Camera* ptr);
void link(Airplane* ptr);
void toggle();
void draw();

} // namespace gui
