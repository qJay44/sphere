#pragma once

#include "objects/Planet.hpp"
#include "engine/AirplaneCamera.hpp"

namespace gui {

void link(Planet* p);
void link(AirplaneCamera* c);
void link(Camera* c);
void toggle();
void draw();

} // namespace gui
