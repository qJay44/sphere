#version 460 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex;

out vec2 texCoord;
out vec3 dirToSun;

uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_planetCenter;

void main() {
	gl_Position = vec4(in_pos, 1.f);
  texCoord = in_tex;
  dirToSun = normalize(u_lightPos - u_planetCenter);
}

