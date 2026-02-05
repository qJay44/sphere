#pragma once

#include "../engine/mesh/Mesh.hpp"

class Light : public Mesh {
public:
  Light(vec3 position, float radius = 30.f, vec3 color = {1.f, 1.f, 1.f});

  const vec3& getPosition() const;
  const vec3& getColor() const;
  const vec3& getWaveLengths() const;

  void update();

  void draw(const Camera* camera, Shader& shader, bool forceNoWireframe = false) const;

private:
  friend struct gui;

  vec3 position;
  float radius;
  vec3 color;

  vec3 waveLengths{700.f, 530.f, 440.f};
};

