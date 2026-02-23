#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inTex;

out vec2 uvs;

uniform mat4 u_model;

void main() {
  uvs = inTex;
  vec4 worldPos = u_model * vec4(inPos, 1.f);

  gl_Position = worldPos;
}

