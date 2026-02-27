#pragma once

#include "../Camera.hpp"
#include "../mesh/Mesh.hpp"

struct Trail : public Mesh {
public:
  Trail();

  float width = 0.1f;
  float duration = 2.f;

  struct Point {
    vec3 position;
    float timestamp;
  };

  std::vector<Point> points;

  void add(const vec3& pos);
  void update(const Camera* cam);
};

