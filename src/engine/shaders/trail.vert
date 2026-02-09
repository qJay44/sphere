#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

out float alpha;

uniform mat4 u_camPV;
uniform mat4 u_model;
uniform float u_fade;

void main() {
  alpha = inColor.r;

  gl_Position = u_camPV * u_model * vec4(inPos, 1.f);
}

