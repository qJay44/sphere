#include "state.hpp"

State _gState = {
  .nearPlane = 0.1f,
  .farPlane  = 100.f,
  .winWidth  = 1200,
  .winHeight = 720,
  .gamma     = 2.2f,
  .time      = 0.f,
  .aaSamples = 8,
};

