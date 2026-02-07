#pragma once

#include "../Shader.hpp"
#include "Light.hpp"

struct PlanetAtmosphere {
  float radius;
  int scatteringPoints = 11;
  int opticalDepthPoints = 11;
  float densityFalloff = 11.f;
  float scatteringStrength = 0.02f;
  float sunIntensity = 1.5f;
  vec3 scatteringCoefficients = vec3(1.f);

  inline static float transmittance(float x, float a, float b) {
    return std::exp(-x * a * b);
  }

  inline static float density(float x, float a) {
    return std::exp(-x * a);
  }

  void update(const Light& light) {
    static float prev{0xFFFFFF};

    if (prev == scatteringStrength)
      return;

    scatteringCoefficients = glm::pow(400.f / light.getWaveLengths(), vec3(4.f)) * scatteringStrength;

    prev = scatteringStrength;
  }

  void setUniforms(Shader& shader) const {
    shader.setUniform3f("u_scatteringCoefficients", scatteringCoefficients);
    shader.setUniform1i("u_scatteringPoints", scatteringPoints);
    shader.setUniform1i("u_opticalDepthPoints", opticalDepthPoints);
    shader.setUniform1f("u_densityFalloff", densityFalloff);
    shader.setUniform1f("u_radius", radius);
    shader.setUniform1f("u_sunIntensity", sunIntensity);
  }
};

