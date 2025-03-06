#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTex;

out vec3 color;
out vec2 tex;

uniform mat4 model;
uniform mat4 cam;

void main() {
  color = inColor;
  tex = inTex;
  gl_Position = cam * model * vec4(inPos, 1.0f);
}
