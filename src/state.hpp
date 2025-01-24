#pragma once

struct State {
  const float nearPlane, farPlane;
  int winWidth, winHeight;
  float gamma;
  double dt;
  u8 aaSamples;
  bool guiFocused;
  int resolution;
};

extern State _gState;
