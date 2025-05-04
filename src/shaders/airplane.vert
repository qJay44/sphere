#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 2) in vec2 inTex;
layout(location = 3) in vec3 inNormal;

out vec2 texCoord;
out vec3 normal;

uniform mat4 model;
uniform mat4 cam;

void main() {
  texCoord = inTex;
  normal = inNormal;
  gl_Position = cam * model * vec4(inPos, 1.0f);
}

