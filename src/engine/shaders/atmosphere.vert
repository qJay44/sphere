#version 460 core

out vec2 texCoord;
out vec3 dirToSun;
out vec3 viewVec;

uniform mat4 u_camInv;
uniform vec3 u_camPos;
uniform vec3 u_lightPos;
uniform vec3 u_planetCenter;

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
  texCoord = vert.xy * 0.5f + 0.5f;
  viewVec = calcViewVec(texCoord);
  dirToSun = normalize(u_lightPos - u_planetCenter);
	gl_Position = vec4(vert, 1.f);
}

