#pragma once

#include "../Shader.hpp"

struct PlanetAtmosphere {
  float radius;
  int scatteringPoints = 40;
  int opticalDepthPoints = 50;
  float densityFalloff = 20.f;
  float scatteringStrength = 0.02f;
  vec3 scatteringCoefficients = vec3(1.f);
  vec3 waveLengths{700.f, 530.f, 440.f};

  bool enable = true;

  inline static float transmittance(float x, float a, float b) {
    return std::exp(-x * a * b);
  }

  inline static float density(float x, float a) {
    return std::exp(-x * a);
  }

  void update() {
    static float prev{0xFFFFFF};

    if (prev == scatteringStrength)
      return;

    scatteringCoefficients = glm::pow(400.f / waveLengths, vec3(4.f)) * scatteringStrength;

    prev = scatteringStrength;
  }

  void setUniforms(Shader& shader) const {
    shader.setUniform3f("u_scatteringCoefficients", scatteringCoefficients);
    shader.setUniform1i("u_scatteringPoints", scatteringPoints);
    shader.setUniform1i("u_opticalDepthPoints", opticalDepthPoints);
    shader.setUniform1f("u_densityFalloff", densityFalloff);
    shader.setUniform1f("u_atmosphereRadius", radius);
    shader.setUniform1i("u_enableAtmosphere", enable);
  }
};

