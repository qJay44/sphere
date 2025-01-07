#pragma once

struct State {
  const float nearPlane, farPlane;
  int winWidth, winHeight;
  float gamma;
  float time;
  u8 aaSamples;
};

extern State _gState;

