#include "global.hpp"

global_cfg _gcfg = {
  .nearPlane = 0.1f,
  .farPlane = 100.f,
  .winWidth = 1'200,
  .winHeight = 720,
  .gamma = 2.2f,
  .dt = 0.f,
  .aaSamples = 8,

  .guiFocused = false,
  .drawWireframe = false,
  .drawNormals = false
};
