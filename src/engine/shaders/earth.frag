#version 460 core

#define INT16_MIN -32768
#define INT16_MAX  32767
#define UINT16_MAX 65535u
#define PI 3.141592265359f

// TODO: Rework water

out vec4 FragColor;
in DATA {
  vec2 texCoord;
  vec4 worldPos;
  float isLand;
} dataIn;

layout(binding = 0) uniform usampler2D u_texIndirection32k;
layout(binding = 1) uniform sampler2DArray u_texVirt32kColors;
layout(binding = 3) uniform sampler2DArray u_texVirt32kNormalmapLand;

layout(binding = 4) uniform sampler2D u_texBathymetry;
layout(binding = 5) uniform sampler2D u_texLandSDF;
layout(binding = 6) uniform sampler2D u_texBorders;
layout(binding = 7) uniform sampler2D u_texNormalmapWave0;
layout(binding = 8) uniform sampler2D u_texNormalmapWave1;

uniform vec2 u_virtualDims;
uniform vec3 u_camPos;
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform vec3 u_bordersColor;
uniform vec3 u_waterDeepColor;
uniform vec3 u_waterShallowColor;
uniform vec3 u_terrainFaceColor;
uniform vec3 u_terrainFaceChunkColor;
uniform float u_time;
uniform float u_radius;
uniform float u_ambient;
uniform float u_specularStrength;
uniform float u_seaLevel;
uniform float u_lightDimScale;
uniform float u_lightMultiplier;
uniform float u_maskTerrainFaceColor;
uniform float u_maskTerrainFaceChunkColor;
uniform float u_waterDeepFactor;
uniform float u_waterDeepEdgeStart;
uniform float u_waterDeepEdgeEnd;
uniform float u_waterSpecularSmoothness;
uniform float u_waterWaveFreq;
uniform float u_waterWaveResMult;
uniform float u_waterShoreWaveFreq;
uniform float u_waterShoreWaveThreshold;
uniform float u_waterShoreWaveThresholdStart;
uniform float u_waterShoreWaveThresholdEnd;
uniform float u_waterShoreWaveAmplitude;
uniform float u_waterShoreWaveNoiseScale;
uniform float u_waterShoreWaveNoiseSpeed;
uniform float u_waterShoreWaveNoiseAmplitude;
uniform float u_triplanarBlendSharpness;

vec3 normalSphere = normalize(dataIn.worldPos.xyz);

// Flip to avoid using vips flipping operations
vec2 globalUV = vec2(
  0.5f - atan(normalSphere.z, normalSphere.x) / (2.f * PI),
  0.5f - (asin(normalSphere.y) / PI)
);

float hash(vec2 p) {
  p = fract(p * vec2(123.34, 456.21));
  p += dot(p, p + 78.233);
  return fract(p.x * p.y);
}

float noise(vec2 p) {
  vec2 i = floor(p);
  vec2 f = fract(p);

  // Four corners
  float a = hash(i);
  float b = hash(i + vec2(1.0, 0.0));
  float c = hash(i + vec2(0.0, 1.0));
  float d = hash(i + vec2(1.0, 1.0));

  // Interpolation
  vec2 u = f * f * (3.0 - 2.0 * f);

  return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

vec3 directionalLight(vec3 normal) {
  vec3 lightDir = normalize(u_lightPos - dataIn.worldPos.xyz);
  float diffuse = max(dot(normal, lightDir), 0.f);

  return u_lightColor * diffuse * u_lightMultiplier;
}

vec3 specularLight(vec3 normal) {
  vec3 viewDir = normalize(u_camPos - dataIn.worldPos.xyz);
  vec3 toLightDir = -normalize(u_lightPos - dataIn.worldPos.xyz);
  vec3 reflDir = reflect(toLightDir, normal); // Reflected light from the world point
  float specular = pow(max(dot(reflDir, viewDir), 0.f), 32.f);

  return u_lightColor * specular * u_specularStrength;
}

vec3 triplanarNormal(sampler2D normalmap, vec3 normal, float timeStep) {
  vec3 absNormal = abs(normal);
  float sum = absNormal.x + absNormal.y + absNormal.z;
  vec3 weights = absNormal / (sum + 1e-16f);

  vec3 colorX = texture(normalmap, dataIn.worldPos.yz * u_waterWaveResMult + timeStep).rgb * 2.f - 1.f;
  vec3 colorY = texture(normalmap, dataIn.worldPos.xz * u_waterWaveResMult + timeStep).rgb * 2.f - 1.f;
  vec3 colorZ = texture(normalmap, dataIn.worldPos.xy * u_waterWaveResMult + timeStep).rgb * 2.f - 1.f;

  weights = pow(weights, vec3(u_triplanarBlendSharpness));
  weights /= (weights.x + weights.y + weights.z);

  return normalize(colorX * weights.x + colorY * weights.y + colorZ * weights.z);
}

vec3 calculateDeepColor(float deepness) {
  float t = pow(deepness, u_waterDeepFactor);
  t = smoothstep(u_waterDeepEdgeStart, u_waterDeepEdgeEnd, t);

  return mix(u_waterDeepColor, u_waterShallowColor, t);
}

float calculateShoreWaves(vec3 normal) {
  float sdf = texture(u_texLandSDF, globalUV).r;
  float dist = 1.f - sdf;
  float factor = smoothstep(u_waterShoreWaveThresholdStart, u_waterShoreWaveThresholdEnd, dist);
  float n = noise(dataIn.worldPos.xy * u_waterShoreWaveNoiseScale + u_time * u_waterShoreWaveNoiseSpeed);
  float wave = sin(dist * u_waterShoreWaveFreq + u_time * 2.f + n * u_waterShoreWaveNoiseAmplitude) * u_waterShoreWaveAmplitude;

  return abs(mix(0.f, wave, factor));
}

vec3 applyMask(vec3 no, vec3 yes, float mask) {
  return yes * mask + (1.f - mask) * no;
}

vec4 textureVirtual(sampler2DArray texVirtual, usampler2D texIndirection) {
  ivec2 tileCoord = ivec2(globalUV * u_virtualDims);
  tileCoord = min(tileCoord, ivec2(u_virtualDims) - 1);

  uint physicalSlot = texelFetch(texIndirection, tileCoord, 0).r;

  if (physicalSlot == 255u)
    return vec4(0.f, 1.f, 0.f, 0.f);

  vec2 localUV = fract(globalUV * u_virtualDims);

  return texture(texVirtual, vec3(localUV, physicalSlot));
}

void main() {
  vec3 color = textureVirtual(u_texVirt32kColors, u_texIndirection32k).rgb;
  vec4 surfaceNormalSample = textureVirtual(u_texVirt32kNormalmapLand, u_texIndirection32k);

  vec3 surfaceNormal = normalize(surfaceNormalSample.rgb * 2.f - 1.f);
  vec3 normalWave0 = triplanarNormal(u_texNormalmapWave0, normalSphere, -u_time * u_waterWaveFreq);
  vec3 normalWave1 = triplanarNormal(u_texNormalmapWave1, normalSphere,  u_time * u_waterWaveFreq);
  vec3 normalWaves = normalize(normalWave0 + normalWave1);
  float border = texture(u_texBorders, globalUV).r;
  float deepness = texture(u_texBathymetry, globalUV).r;

  float isLand = round(deepness); // TODO: Probably not that easy
  float isWater = 1.f - isLand;

  surfaceNormal *= -1.f;

  color = isWater * calculateDeepColor(deepness) + isLand * color;
  color *= isLand * directionalLight(surfaceNormal) + isWater;
  color += isWater * calculateShoreWaves(normalSphere);
  color += isWater * specularLight(normalWaves);
  color += border * u_bordersColor;

  color = applyMask(color, u_terrainFaceColor, u_maskTerrainFaceColor);
  color = applyMask(color, u_terrainFaceChunkColor, u_maskTerrainFaceChunkColor);

  FragColor = vec4(color, 1.f);
}

