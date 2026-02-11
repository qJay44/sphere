#version 460 core

#define INT16_MIN -32768
#define INT16_MAX  32767
#define UINT16_MAX 65535u

out vec4 FragColor;

in DATA {
  vec3 normal;
  vec4 worldPos;
} dataIn;

layout(binding = 0) uniform isamplerCube u_heightmapsWater;
layout(binding = 1) uniform usamplerCube u_distanceFieldsWater;
layout(binding = 2) uniform samplerCube u_normalheightmapsLand;
layout(binding = 3) uniform samplerCube u_worldColors;
layout(binding = 4) uniform samplerCube u_borders;
layout(binding = 5) uniform sampler2D u_normalmapWave0;
layout(binding = 6) uniform sampler2D u_normalmapWave1;

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
uniform float u_specularLight;
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

vec3 directionalLight(vec3 normal, float isWater) {
  vec3 lightDistVec = u_lightPos - dataIn.worldPos.xyz;
  float lightDist = length(lightDistVec);

  vec3 lightDirection = normalize(lightDistVec);
  float diffuse = max(dot(normal, lightDirection), 0.f);

  vec3 viewDirection = normalize(u_camPos - dataIn.worldPos.xyz);
  vec3 reflectionDirection = reflect(-lightDirection, normal);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 8);
  float specular = specAmount * u_specularLight * isWater;
  float lightAmount = (diffuse + u_ambient + specular);
  float intensity = smoothstep(u_radius * u_lightDimScale, 0.f, lightDist);

  return u_lightColor * lightAmount * intensity * u_lightMultiplier;
}

vec3 triplanarNormal(sampler2D normalmap, float timeStep) {
  vec3 absNormal = abs(dataIn.normal);
  float sum = absNormal.x + absNormal.y + absNormal.z;
  vec3 weights = absNormal / (sum + 1e-16f);

  vec3 colorX = texture(normalmap, dataIn.worldPos.yz * u_waterWaveResMult + timeStep).rgb * 2.f - 1.f;
  vec3 colorY = texture(normalmap, dataIn.worldPos.xz * u_waterWaveResMult + timeStep).rgb * 2.f - 1.f;
  vec3 colorZ = texture(normalmap, dataIn.worldPos.xy * u_waterWaveResMult + timeStep).rgb * 2.f - 1.f;

  weights = pow(weights, vec3(u_triplanarBlendSharpness));
  weights /= (weights.x + weights.y + weights.z);

  return normalize(colorX * weights.x + colorY * weights.y + colorZ * weights.z);
}

vec3 calculateDeepColor(int deepness) {
  float d = float(deepness) / INT16_MAX * 0.5f + 0.5f;
  float t = pow(d, u_waterDeepFactor);
  t = smoothstep(u_waterDeepEdgeStart, u_waterDeepEdgeEnd, t);

  return mix(u_waterDeepColor, u_waterShallowColor, t);
}

float calculateShoreWaves() {
  const uint distFromShore = texture(u_distanceFieldsWater, dataIn.normal).r;
  float dist = 1.f - float(distFromShore) / UINT16_MAX;
  float factor = smoothstep(u_waterShoreWaveThresholdStart, u_waterShoreWaveThresholdEnd, dist);
  float n = noise(dataIn.worldPos.xy * u_waterShoreWaveNoiseScale + u_time * u_waterShoreWaveNoiseSpeed);
  float wave = sin(dist * u_waterShoreWaveFreq + u_time * 2.f + n * u_waterShoreWaveNoiseAmplitude) * u_waterShoreWaveAmplitude;

  return abs(mix(0.f, wave, factor));
}

vec3 applyMask(vec3 no, vec3 yes, float mask) {
  return yes * mask + (1.f - mask) * no;
}

void main() {
  vec4 normalheightSample = texture(u_normalheightmapsLand, dataIn.normal);
  vec3 surfaceNormal = normalize(normalheightSample.rgb * 2.f - 1.f);
  vec3 color = texture(u_worldColors, dataIn.normal).rgb;
  vec3 normalWave0 = triplanarNormal(u_normalmapWave0, -u_time * u_waterWaveFreq);
  vec3 normalWave1 = triplanarNormal(u_normalmapWave1,  u_time * u_waterWaveFreq);
  vec3 normalWaves = normalize(normalWave0 + normalWave1);
  float border = texture(u_borders, dataIn.normal).r;
  int deepness = texture(u_heightmapsWater, dataIn.normal).r; // [-32768, 32767]

  // water: -1; land: 0 and 1
  float isWater = floor(-sign(deepness) * 0.5f + 0.5f);
  float isLand = 1.f - isWater;

  surfaceNormal *= -1.f;

  color = isWater * calculateDeepColor(deepness) + isLand * color;
  color *= isLand * directionalLight(surfaceNormal, 0.f) + isWater;
  color += isWater * calculateShoreWaves() * directionalLight(surfaceNormal, 1.f);
  color *= isWater * directionalLight(normalWaves, 1.f) + isLand;
  color += border * u_bordersColor;

  color = applyMask(color, u_terrainFaceColor, u_maskTerrainFaceColor);
  color = applyMask(color, u_terrainFaceChunkColor, u_maskTerrainFaceChunkColor);

  FragColor = vec4(color, 1.f);
}

