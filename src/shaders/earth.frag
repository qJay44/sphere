#version 460 core

out vec4 FragColor;

in vec3 vertPos;
in vec3 vertPosOriginal;
in vec3 defaultNormal;
in vec3 lightPos;
in vec3 camPos;
in vec2 texCoord;

uniform vec3 u_debug_terrainFaceColor;
uniform vec3 u_debug_terrainFaceChunkColor;

uniform vec3 u_lightColor;
uniform vec3 u_bordersColor;
uniform vec3 u_waterShallowColor;
uniform vec3 u_waterDeepColor;
uniform isamplerCube u_heightmapsWater;
uniform usamplerCube u_distanceFieldsWater;
uniform samplerCube u_normalheightmapsLand;
uniform samplerCube u_worldColors;
uniform samplerCube u_borders;
uniform sampler2D u_normalmapWave0;
uniform sampler2D u_normalmapWave1;
uniform float u_lightMultiplier;
uniform float u_ambient;
uniform float u_specularLight;
uniform float u_waterDeepFactor;
uniform float u_waterDeepEdgeStart;
uniform float u_waterDeepEdgeEnd;
uniform float u_waterSpecularSmoothness;
uniform float u_waterWaveFreq;
uniform float u_waterWaveResMult;
uniform float u_waterShoreWaveScale;
uniform float u_waterShoreWaveFreq;
uniform float u_waterShoreWaveThreshold;
uniform float u_waterShoreWaveAmplitudeScale;
uniform float u_waterShoreWaveSmoothnessStart;
uniform float u_waterShoreWaveSmoothnessEnd;
uniform float u_time;
uniform float u_triplanarBlendSharpness;

vec3 directionalLight(vec3 normal) {
  vec3 lightDirection = normalize(lightPos - vertPos);
  float diffuse = max(dot(normal, lightDirection), 0.f);

  vec3 viewDirection = normalize(camPos - vertPos);
  vec3 reflectionDirection = reflect(-lightDirection, normal);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 8);
  float specular = specAmount * u_specularLight;

  return (diffuse + u_ambient + specular) * u_lightMultiplier * u_lightColor;
}

vec3 triplanarNormal(sampler2D normalmap, float timeStep) {
  vec3 absNormal = abs(defaultNormal);
  float sum = absNormal.x + absNormal.y + absNormal.z;
  vec3 weights = absNormal / (sum + 1e-16f);

  vec3 colorX = texture(normalmap, vertPosOriginal.yz * u_waterWaveResMult + timeStep).rgb * 2.f - 1.f;
  vec3 colorY = texture(normalmap, vertPosOriginal.xz * u_waterWaveResMult + timeStep).rgb * 2.f - 1.f;
  vec3 colorZ = texture(normalmap, vertPosOriginal.xy * u_waterWaveResMult + timeStep).rgb * 2.f - 1.f;

  weights = pow(weights, vec3(u_triplanarBlendSharpness));
  weights /= (weights.x + weights.y + weights.z);

  return colorX * weights.x + colorY * weights.y + colorZ * weights.z;
}

float calculateSpecular(vec3 normal) {
  vec3 lightDirection = normalize(lightPos - vertPos);
  float specularAngle = max(acos(dot(lightDirection, normal)), 0.f);
  float specularExponent = specularAngle / u_waterSpecularSmoothness;
  float specularHighlight = exp(-specularExponent * specularExponent);

  return specularHighlight;
}

vec3 calculateDeepColor(int deepness) {
  float d = deepness / 32768.f * 0.5f + 0.5f;
  float t = pow(d, u_waterDeepFactor);
  t = smoothstep(u_waterDeepEdgeStart, u_waterDeepEdgeEnd, t);

  return mix(u_waterDeepColor, u_waterShallowColor, t);
}

float calculateShoreWaves() {
  float distFromShore = float(texture(u_distanceFieldsWater, defaultNormal).r);
  float shoreProximity = smoothstep(u_waterShoreWaveThreshold, u_waterShoreWaveThreshold * 0.5f, distFromShore);
  float waveAmplitude = 0.f;
  if (shoreProximity > 0.f) {
    waveAmplitude = abs(sin(u_waterShoreWaveScale * (distFromShore + u_time * u_waterShoreWaveFreq))) * shoreProximity;
    waveAmplitude = smoothstep(u_waterShoreWaveSmoothnessStart, u_waterShoreWaveSmoothnessEnd, waveAmplitude);
  }

  return waveAmplitude * u_waterShoreWaveAmplitudeScale;
}

void main() {
  vec3 normal = texture(u_normalheightmapsLand, defaultNormal).rgb * 2.f - 1.f;
  vec3 color = texture(u_worldColors, defaultNormal).rgb;
  vec3 normalWave0 = triplanarNormal(u_normalmapWave0, -u_time * u_waterWaveFreq);
  vec3 normalWave1 = triplanarNormal(u_normalmapWave1,  u_time * u_waterWaveFreq);
  vec3 normalWaves = normalize(normalWave0 + normalWave1);
  int deepness = texture(u_heightmapsWater, defaultNormal).r; // [-32768, 32767]
  float border = texture(u_borders, defaultNormal).r;
  float isWater = 1.f - (sign(deepness) * 0.5f + 0.5f);
  float isLand = 1.f - isWater;

  color = isWater * calculateDeepColor(deepness) + isLand * color;
  color *= isLand * directionalLight(normal) + isWater;
  color += isWater * calculateShoreWaves();
  color *= isWater * directionalLight(normalWaves) + isLand;
  color += isWater * calculateSpecular(normalWaves);
  color += border * u_bordersColor;

  FragColor = vec4(color, 1.f);
}

