#version 460 core

#define FLOAT_MAX 3.4028235e38f

in vec2 texCoord;
in vec3 viewVec;
in vec3 dirToSun;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_screenColorTex;
layout(binding = 1) uniform sampler2D u_screenDepthTex;

uniform vec3 u_camPos;
uniform vec3 u_camForward;
uniform float u_camNear;
uniform float u_camFar;

uniform vec3 u_planetCenter;
uniform vec3 u_scatteringCoefficients;
uniform int u_scatteringPoints;
uniform int u_opticalDepthPoints;
uniform float u_planetRadius;
uniform float u_radius;
uniform float u_densityFalloff;
uniform float u_sunIntensity;

float linearizeDepth(float depth) {
  float z = depth * 2.f - 1.f;

  return (2.f * u_camNear * u_camFar) / (u_camFar + u_camNear - z * (u_camFar - u_camNear));
}

vec2 raySphere(vec3 center, float radius, vec3 rayOrigin, vec3 rayDir) {
  vec3 offset = rayOrigin - center;
  float a = 1.f;
  float b = 2.f * dot(offset, rayDir);
  float c = dot(offset, offset) - radius * radius;
  float discriminant = b * b - 4.f * a * c;

  if (discriminant > 0.f) {
    float s = sqrt(discriminant);
    float dstToSphereNear = max(0.f, (-b - s) / (2.f * a));
    float dstToSphereFar = (-b + s) / (2.f * a);

    if (dstToSphereFar >= 0.f)
      return vec2(dstToSphereNear, dstToSphereFar - dstToSphereNear);
  }

  return vec2(FLOAT_MAX, 0.f);
}

float densityAtPoint(vec3 densitySamplePoint) {
  float heightAboveSurface = length(densitySamplePoint - u_planetCenter) - u_planetRadius;
  float heightNorm = heightAboveSurface / (u_radius - u_planetRadius);
  float localDensity = exp(-heightNorm * u_densityFalloff) * (1.f - heightNorm);

  return localDensity;
}

float opticalDepth(vec3 rayOrigin, vec3 rayDir, float rayLength) {
  vec3 densitySamplePoint = rayOrigin;
  float stepSize = rayLength / (u_opticalDepthPoints - 1);
  float res = 0.f;

  for (int i = 0; i < u_opticalDepthPoints; i++) {
    float localDensity = densityAtPoint(densitySamplePoint);
    res += localDensity * stepSize;
    densitySamplePoint += rayDir * stepSize;
  }

  return res;
}

vec3 calcLight(vec3 rayOrigin, vec3 rayDir, float rayLength, vec3 originalColor) {
  vec3 inScatteringPoint = rayOrigin;
  vec3 inScatteredLight = vec3(0.f);
  float stepSize = rayLength / float(u_scatteringPoints - 1);
  float viewRayOpticalDepth = 0.f;

  for (int i = 0; i < u_scatteringPoints; i++) {
    float sunRayLength = raySphere(u_planetCenter, u_radius, inScatteringPoint, dirToSun).y;
    float sunRayOpticalDepth = opticalDepth(inScatteringPoint, dirToSun, sunRayLength);
    viewRayOpticalDepth = opticalDepth(inScatteringPoint, -rayDir, stepSize * i);
    float localDensity = densityAtPoint(inScatteringPoint);
    vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * u_scatteringCoefficients);

    inScatteredLight += localDensity * transmittance * u_scatteringCoefficients * stepSize;
    inScatteringPoint += rayDir * stepSize;
  }

  float originalColorTransmittance = exp(-viewRayOpticalDepth);

  return originalColor * originalColorTransmittance + inScatteredLight * u_sunIntensity;
}

void main() {
  float viewVecLength = length(viewVec);
  vec3 rayOrigin = u_camPos;
  vec3 rayDir = viewVec / viewVecLength;
  vec3 color = texture(u_screenColorTex, texCoord).rgb;
  vec2 hitInfo = raySphere(u_planetCenter, u_radius, rayOrigin, rayDir);

  float sceneDepthNonLinear = texture(u_screenDepthTex, texCoord).r;
  float eyeDepth = linearizeDepth(sceneDepthNonLinear);
  float sceneDepth = eyeDepth / dot(rayDir, u_camForward);

  float dstToAtmosphere = hitInfo.x;
  float dstThroughAtmosphere = min(hitInfo.y, sceneDepth - dstToAtmosphere);

  if (dstThroughAtmosphere > 0.f) {
    vec3 pointInAtmosphere = rayOrigin + rayDir * dstToAtmosphere;
    vec3 light = calcLight(pointInAtmosphere, rayDir, dstThroughAtmosphere, color);

    color += light; // ugh.......
  }

  FragColor = vec4(color, 1.f);
}

