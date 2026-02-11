#version 460 core

layout (quads, equal_spacing, ccw) in;

in vec2 uvsCoord[];

out DATA {
  vec3 normal;
  vec4 worldPos;
} dataOut;

layout(binding = 2) uniform samplerCube u_normalheightmapsLand;

uniform mat4 u_camPV;
uniform float u_heightmapScale;

void main() {
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;

  vec2 uv0 = mix(uvsCoord[0], uvsCoord[1], u);
  vec2 uv1 = mix(uvsCoord[3], uvsCoord[2], u);
  vec2 texCoord = mix(uv0, uv1, v);

  vec4 p0 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, u);
  vec4 p1 = mix(gl_in[3].gl_Position, gl_in[2].gl_Position, u);
  vec4 worldPos = mix(p0, p1, v);
  vec3 normal = normalize(worldPos.xyz);

  float height = texture(u_normalheightmapsLand, normal).a;
  worldPos.xyz += normal * height * u_heightmapScale;

  dataOut.normal = normal;
  dataOut.worldPos = worldPos;

  gl_Position = u_camPV * worldPos;
}

