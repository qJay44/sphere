#pragma once

#include <vector>

#include "../engine/Camera.hpp"

class Trail {
public:
  Trail() = default;
  Trail(vec3 color);

  void add(const vec3& pos, const float& duration);
  void update();

  void draw(const Camera* cam, const Shader& shader) const;

private:
  friend struct gui;

  vec3 color = vec3(1.f);

  struct TrailPoint {
    vec3 position;
    float timestamp;
    float duration;
  };

  std::vector<TrailPoint> trailPoints;
};

