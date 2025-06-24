#version 460 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex;

out vec3 vertPos;
out vec2 texCoord;

uniform vec3 u_camPos;

void main() {
	gl_Position = vec4(in_pos, 1.f);
  vertPos = in_pos;
  texCoord = in_tex;
}

