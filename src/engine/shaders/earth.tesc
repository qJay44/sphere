#version 460 core

layout (vertices = 4) out;

in vec2 uvs[];
out vec2 uvsCoord[];

uniform vec3 u_camPos;
uniform float u_div;
uniform float u_camFar;

void main() {
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  uvsCoord[gl_InvocationID] = uvs[gl_InvocationID];

  vec3 p0 = gl_in[0].gl_Position.xyz;
  vec3 p1 = gl_in[1].gl_Position.xyz;
  vec3 p2 = gl_in[2].gl_Position.xyz;
  vec3 p3 = gl_in[3].gl_Position.xyz;

  vec3 pos = (p0 + p1 + p2 + p3) / 4.f;
  float dist = distance(pos, u_camPos);
  // float tessLvl = u_div * (1.f - smoothstep(0.f, u_camFar, dist));
  float tessLvl = u_div;

  gl_TessLevelOuter[0] = tessLvl;
  gl_TessLevelOuter[1] = tessLvl;
  gl_TessLevelOuter[2] = tessLvl;
  gl_TessLevelOuter[3] = tessLvl;

  gl_TessLevelInner[0] = tessLvl;
  gl_TessLevelInner[1] = tessLvl;
}

