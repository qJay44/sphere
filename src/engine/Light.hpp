#pragma once

#include "Shader.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"

class Light {
public:
  Light(vec3 position, vec4 color = {1.f, 1.f, 1.f, 1.f});

  const vec3& getPosition() const;
  const vec4& getColor() const;

  void draw(const Camera& camera, const Shader& shader) const;

private:
  vec3 position;
  vec4 color;
  Mesh mesh;
};

