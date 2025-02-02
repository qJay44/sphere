#pragma once

struct global_cfg {
  const float nearPlane, farPlane;
  int winWidth, winHeight;
  float gamma;
  double dt;
  u8 aaSamples;
  ThreadPool tp;

  bool guiFocused;
  bool drawWireframe;
  bool drawNormals;
};

extern global_cfg _gcfg;
