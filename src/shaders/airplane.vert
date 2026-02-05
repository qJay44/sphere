#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 2) in vec2 inTex;
layout(location = 3) in vec3 inNormal;

out vec4 worldPos;
out vec3 normal;
out vec2 texCoord;

uniform mat4 u_camPV;
uniform mat4 u_model;

void main() {
  worldPos = u_model * vec4(inPos, 1.f);
  texCoord = inTex;
  normal = inNormal;
  gl_Position = u_camPV * worldPos;
}

