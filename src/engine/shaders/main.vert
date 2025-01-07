#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inCol;
layout (location = 2) in vec2 inTex;
layout (location = 3) in vec3 inNormal;

out vec3 color;
out vec2 texCoord;
out vec3 normal;

uniform mat4 model;
uniform mat4 cam;

void main() {
  vec3 vertPos = vec3(model * vec4(inPos, 1.f));
  color = inCol;
  texCoord = inTex;
  normal = inNormal;
  gl_Position = cam * vec4(vertPos, 1.f);
}

