#pragma once

#include "../Shader.hpp"
#include "../Camera.hpp"
#include "../mesh/Mesh.hpp"

struct Sun {
  float focus;
  float intensity;  // radians
  float yaw = PI_2; // radians
  float pitch = 0.f;
  vec3 color{1.f};

  vec3 dir{1.f, 0.f, 0.f}; // Towards light source

  void updateDir() {
    dir = normalize(vec3{
      cos(yaw) * cos(pitch),
      sin(pitch),
      sin(yaw) * cos(pitch)
    });
  }

  void setUniforms(Shader& shader) {
    shader.setUniform1f("u_sunFocus", focus);
    shader.setUniform1f("u_sunIntensity", intensity);
    shader.setUniform3f("u_sunDir", dir);
    shader.setUniform3f("u_sunColor", color);
  }

  void draw(const Camera* cam, Shader& shader) {
    setUniforms(shader);
    shader.setUniformMatrix4f("u_camInv", cam->getProjViewInv());

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);

    Mesh::drawScreen(cam, shader);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
  }
};

