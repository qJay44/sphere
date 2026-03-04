#version 460 core

in vec4 worldPos;
in vec2 texCoord;
in vec3 normal;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_diffuse0;

uniform vec3 u_camPos;
uniform vec3 u_sunDir;
uniform vec3 u_sunColor;

vec3 color = texture(u_diffuse0, texCoord).rgb;

vec3 directionalLight() {
  float diffuse = max(dot(normal, u_sunDir), 0.f);
  float ambient = 0.2f;

  float specularLight = 0.5f;
  vec3 viewDirection = normalize(u_camPos - worldPos.xyz);
  vec3 reflectionDirection = reflect(-u_sunDir, normal);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 8);
  float specular = specAmount * specularLight;

  return color * (diffuse + ambient + specular) * u_sunColor;
}

void main() {
  FragColor = vec4(directionalLight(), 1.f);
}

