#pragma once

#include "objects/Planet.hpp"
#include "engine/ArcballCamera.hpp"

namespace gui {
void link(Planet* p);
void link(ArcballCamera* c);
void toggle();
void draw();
} // namespace gui
