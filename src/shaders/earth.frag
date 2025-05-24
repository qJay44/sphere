#version 460 core

out vec4 FragColor;

in vec3 vertPos;
in vec2 texCoord;
in vec3 lightPos;
in vec3 camPos;
in float u0;
in float u1;
in float idx;

uniform vec3 u_lightColor;
uniform vec3 u_borderColor;
uniform sampler2DArray u_normalheightmaps;
uniform sampler2DArray u_worldColors;
uniform sampler2DArray u_borders;
uniform float u_lightMultiplier;
uniform float u_ambient;
uniform float u_specularLight;

vec2 texUV = vec2((fwidth(u0) < fwidth(u1) - 0.0001f) ? u0 : u1, texCoord.y);
vec3 normal = texture(u_normalheightmaps, vec3(texUV, idx)).rgb * 2.f - 1.f;
vec3 color = texture(u_worldColors, vec3(texUV, idx)).rgb;
vec3 border = vec3(texture(u_borders, vec3(texUV, idx)).r);

vec3 directionalLight() {
  vec3 lightDirection = normalize(lightPos - vertPos);
  float diffuse = max(dot(normal, lightDirection), 0.f);

  vec3 viewDirection = normalize(camPos - vertPos);
  vec3 reflectionDirection = reflect(-lightDirection, normal);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 8);
  float specular = specAmount * u_specularLight;

  return color * (diffuse + u_ambient + specular) * u_lightMultiplier * u_lightColor;
}

void main() {
  color = directionalLight();
  color += border * u_borderColor;

  FragColor = vec4(color, 1.f);
}

