#version 460 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex;

out vec2 texCoord;
out vec3 dirToSun;
out vec3 viewVec;

uniform mat4 u_camInv;
uniform vec3 u_lightPos;
uniform vec3 u_camPos;
uniform vec3 u_planetCenter;

vec3 calcViewVec() {
  vec2 ndc = texCoord * 2.f - 1.f;
  vec4 clipPos = vec4(ndc, -1.f, 1.f);
  vec4 worldPos = u_camInv * clipPos;
  worldPos /= worldPos.w;

  return worldPos.xyz - u_camPos;
}

void main() {
	gl_Position = vec4(in_pos, 1.f);
  texCoord = in_tex;
  viewVec = calcViewVec();
  dirToSun = normalize(u_lightPos - u_planetCenter);
}

