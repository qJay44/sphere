#pragma once

#include "../engine/mesh/Mesh.hpp"

class Light : public Mesh {
public:
  Light(vec3 position, vec4 color = {1.f, 1.f, 1.f, 1.f});

  const vec3& getPosition() const;
  const vec4& getColor() const;

private:
  vec3 position;
  vec4 color;
};
