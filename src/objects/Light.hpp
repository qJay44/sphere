#pragma once

#include "../engine/mesh/Mesh.hpp"

class Light : public Mesh {
public:
  Light(vec3 position, float size = 1.f, vec3 color = {1.f, 1.f, 1.f});

  const vec3& getPosition() const;
  const vec3& getColor() const;

private:
  friend struct gui;

  vec3 position;
  vec3 color;
};

