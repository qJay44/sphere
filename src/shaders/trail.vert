#version 460 core

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_color;

out vec4 color;

uniform mat4 u_cam;
uniform vec3 u_closestPos;
uniform float u_alphaFactor;

void main() {
  float len = length(in_pos - u_closestPos);
  color = vec4(in_color, 1.f - len * u_alphaFactor);
  gl_Position = u_cam * vec4(in_pos, 1.f);
}

