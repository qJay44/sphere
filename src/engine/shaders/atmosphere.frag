#version 460 core

#define FLOAT_MAX 3.4028235e38f
#define PI 3.141592265359f

in vec2 v_uv;
in vec3 v_viewVec;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_screenColorTex;
layout(binding = 1) uniform sampler2D u_screenDepthTex;
layout(binding = 2) uniform sampler2D u_bakedOpticalDepth;

uniform vec3 u_camPos;
uniform vec3 u_camForward;
uniform float u_camNear;
uniform float u_camFar;

uniform vec3 u_sunDir;
uniform vec3 u_sunColor;
uniform vec3 u_planetCenter;
uniform vec3 u_scatteringCoefficients;
uniform float u_gamma;
uniform float u_planetRadius;
uniform float u_atmosphereRadius;
uniform float u_densityFalloff;
uniform float u_sunFocus;
uniform float u_sunIntensity;
uniform bool u_enableAtmosphere;
uniform int u_scatteringPoints;
uniform int u_opticalDepthPoints;

struct Ray {
  vec3 origin;
  vec3 dir;
};

float linearizeDepth(float depth) {
  float z = depth * 2.f - 1.f;
  float n = u_camNear;
  float f = u_camFar;

  return (2.f * n * f) / (f + n - z * (f - n));
}

vec3 applyMask(vec3 no, vec3 yes, float mask) {
  return yes * mask + (1.f - mask) * no;
}

vec2 raySphere(vec3 center, float radius, Ray ray) {
  vec3 offset = ray.origin - center;
  float a = 1.f;
  float b = 2.f * dot(offset, ray.dir);
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
  float heightNorm = heightAboveSurface / (u_atmosphereRadius - u_planetRadius);
  float localDensity = exp(-heightNorm * u_densityFalloff) * (1.f - heightNorm);

  return localDensity;
}

float opticalDepth(vec3 p) {
  float height = length(u_planetCenter - p) - u_planetRadius;
  float heightNorm = height / (u_atmosphereRadius - u_planetRadius);
  float angleNorm = dot(normalize(p - u_planetCenter), u_sunDir) * 0.5f + 0.5f;
  vec2 uv = vec2(angleNorm, heightNorm);

  return texture(u_bakedOpticalDepth, uv).r;
}

vec3 calcLight(Ray ray, float rayLength, vec3 originalColor) {
  vec3 inScatteringPoint = ray.origin;
  vec3 inScatteredLight = vec3(0.f);
  float stepSize = rayLength / float(u_scatteringPoints - 1);
  float viewRayOpticalDepth = 0.f;

  for (int i = 0; i < u_scatteringPoints; i++) {
    float sunRayOpticalDepth = opticalDepth(inScatteringPoint);
    float localDensity = densityAtPoint(inScatteringPoint);
    viewRayOpticalDepth += localDensity * stepSize;
    vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * u_scatteringCoefficients);

    inScatteredLight += localDensity * transmittance * u_scatteringCoefficients * stepSize;
    inScatteringPoint += ray.dir * stepSize;
  }

  vec3 originalColorTransmittance = exp(-viewRayOpticalDepth * u_scatteringCoefficients);

  return originalColor * originalColorTransmittance + inScatteredLight * u_sunIntensity;
}

void main() {
  vec3 color = texture(u_screenColorTex, v_uv).rgb;

  if (!u_enableAtmosphere) {
    FragColor = vec4(color, 1.f);
    return;
  }

  Ray ray;
  ray.origin = u_camPos;
  ray.dir = normalize(v_viewVec);

  vec2 hitInfo = raySphere(u_planetCenter, u_atmosphereRadius, ray);

  float sceneDepthNonLinear = texture(u_screenDepthTex, v_uv).r;
  float eyeDepth = linearizeDepth(sceneDepthNonLinear);
  float sceneDepth = eyeDepth / dot(ray.dir, u_camForward);

  float dstToAtmosphere = hitInfo.x;
  float dstThroughAtmosphere = min(hitInfo.y, sceneDepth - dstToAtmosphere);

  if (dstThroughAtmosphere > 0.f) {
    ray.origin += ray.dir * dstToAtmosphere;
    vec3 light = calcLight(ray, dstThroughAtmosphere, color);

    color = light;
  }

  if (sceneDepth > u_camFar) {
    float sunDot = max(dot(ray.dir, u_sunDir), 0.f);
    float sun = pow(sunDot, u_sunFocus) * u_sunIntensity;

    color += sun * u_sunColor;
  }

  FragColor = vec4(color, 1.f);
}

