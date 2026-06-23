#version 460 core

out vec2 v_uv;
out vec3 v_viewVec;

uniform mat4 u_camInv;
uniform vec3 u_camPos;

const vec2 vertices[] = vec2[](
  vec2(-1, -1),
  vec2(-1,  1),
  vec2( 1,  1),
  vec2( 1,  1),
  vec2( 1, -1),
  vec2(-1, -1)
);

vec3 calcViewVec(vec2 uv) {
  vec2 ndc = uv * 2.f - 1.f;
  vec4 clipPos = vec4(ndc, -1.f, 1.f);
  vec4 worldPos = u_camInv * clipPos;
  worldPos /= worldPos.w;

  return worldPos.xyz - u_camPos;
}

void main() {
  vec3 vert = vec3(vertices[gl_VertexID], 0.f);

  v_uv = vert.xy * 0.5f + 0.5f;
  v_viewVec = calcViewVec(v_uv);
	gl_Position = vec4(vert, 1.f);
}

