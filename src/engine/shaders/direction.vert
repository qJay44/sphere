#version 460 core

uniform mat4 u_camPV;
uniform vec3 u_pos;
uniform vec3 u_dir;

const float dirMask[2] = float[2](0.f, 1.f);

void main() {
  vec3 pos = u_pos + dirMask[gl_VertexID] * u_dir;
	gl_Position = u_camPV * vec4(pos, 1.f);
}

