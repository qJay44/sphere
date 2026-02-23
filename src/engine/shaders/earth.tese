#version 460 core

#define PI 3.141592265359f

layout (quads, equal_spacing, ccw) in;

in vec2 uvsCoord[];

out DATA {
  vec2 texCoord;
  vec4 worldPos;
  float isLand;
} dataOut;

layout(binding = 2) uniform sampler2DArray u_texHeightmapLand;
layout(binding = 3) uniform usampler2D u_texHeightmapLandIndirection;

uniform vec2 u_virtualDims;
uniform mat4 u_camPV;
uniform float u_heightmapScale;

const float seaLevelNorm = 143.f / 255.f;

vec4 textureVirtual(sampler2DArray texPhysical, usampler2D texVirtual, vec3 normal) {
  vec2 globalUV = vec2(
    atan(normal.z, normal.x) / (2.f * PI) + 0.5f,
    asin(normal.y) / PI + 0.5f
  );

  // Flip to avoid using vips flipping operations
  globalUV = 1.f - globalUV;

  ivec2 tileCoord = ivec2(globalUV * u_virtualDims);
  tileCoord = min(tileCoord, ivec2(u_virtualDims) - 1);

  uint physicalSlot = texelFetch(texVirtual, tileCoord, 0).r;

  if (physicalSlot == 255u)
    return vec4(0.f, 1.f, 0.f, 0.f);

  vec2 localUV = fract(globalUV * u_virtualDims);

  return texture(texPhysical, vec3(localUV, physicalSlot));
}

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

  float height = textureVirtual(u_texHeightmapLand, u_texHeightmapLandIndirection, normal).r;
  worldPos.xyz += normal * height * u_heightmapScale;

  dataOut.texCoord = texCoord;
  dataOut.worldPos = worldPos;
  dataOut.isLand = floor(height / seaLevelNorm);

  gl_Position = u_camPV * worldPos;
}

