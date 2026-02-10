#pragma once

#include "../Shader.hpp"
#include "Light.hpp"

struct PlanetAtmosphere {
  float radius;
  int scatteringPoints = 40;
  int opticalDepthPoints = 50;
  float densityFalloffR = 1.f / 8.f;
  float densityFalloffM = 1.f / 12.f;
  float scatteringStrengthR = 0.02f;
  float scatteringStrengthM = 0.02f;
  float sunIntensity = 2.f;
  vec3 scatteringCoefficientsR = vec3(1.f);
  vec3 scatteringCoefficientsM = vec3(1.f);

  bool useGammaCorrection = false;

  inline static float transmittance(float x, float a, float b) {
    return std::exp(-x * a * b);
  }

  inline static float density(float x, float a) {
    return std::exp(-x * a);
  }

  void update(const Light& light) {
    static float prevR{0xFFFFFF};
    static float prevM{0xFFFFFF};

    if (prevR == scatteringStrengthR && prevM == scatteringStrengthM)
      return;

    scatteringCoefficientsR = glm::pow(400.f / light.getWaveLengths(), vec3(4.f)) * scatteringStrengthR;
    scatteringCoefficientsM = glm::pow(400.f / light.getWaveLengths(), vec3(4.f)) * scatteringStrengthM;

    prevR = scatteringStrengthR;
    prevM = scatteringStrengthM;
  }

  void setUniforms(Shader& shader) const {
    shader.setUniform3f("u_scatteringCoefficientsR", scatteringCoefficientsR);
    shader.setUniform3f("u_scatteringCoefficientsM", scatteringCoefficientsM);
    shader.setUniform1i("u_scatteringPoints", scatteringPoints);
    shader.setUniform1i("u_opticalDepthPoints", opticalDepthPoints);
    shader.setUniform1f("u_densityFalloffR", densityFalloffR);
    shader.setUniform1f("u_densityFalloffM", densityFalloffM);
    shader.setUniform1f("u_atmosphereRadius", radius);
    shader.setUniform1f("u_sunIntensity", sunIntensity);
    shader.setUniform1f("u_maskGammaCorrection", useGammaCorrection);
  }
};

