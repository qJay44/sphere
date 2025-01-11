#version 460 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;

out vec3 color;

uniform mat4 model;
uniform mat4 cam;

void main() {
  color = inColor;
	gl_Position = cam * model * vec4(inPos, 1.0f);
}

