#version 460 core

in vec3 vertPos;
in vec2 texCoord;
in vec3 normal;

out vec4 FragColor;

uniform vec3 camPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform sampler2D diffuse0;

vec4 directionalLight() {
  vec3 n = normalize(normal);
  vec3 lightDirection = normalize(lightPos - vertPos);
  float diffuse = max(dot(n, lightDirection), 0.f);
  float ambient = 0.2f;

  float specularLight = 0.5f;
  vec3 viewDirection = normalize(camPos - vertPos);
  vec3 reflectionDirection = reflect(-lightDirection, n);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 8);
  float specular = specAmount * specularLight;

  return (diffuse + ambient) * vec4(lightColor, 1.f);
}

void main() {
  FragColor = vec4(texture(diffuse0, texCoord).rgb, 1.f) * directionalLight();
}

