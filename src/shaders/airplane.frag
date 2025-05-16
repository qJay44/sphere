#version 460 core

in vec3 vertPos;
in vec2 texCoord;
in vec3 normal;

out vec4 FragColor;

uniform vec3 u_camPos;
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform sampler2D u_diffuse0;

vec3 color = texture(u_diffuse0, texCoord).rgb;

vec4 directionalLight() {
  vec3 lightDirection = normalize(u_lightPos - vertPos);
  float diffuse = max(dot(normal, lightDirection), 0.f);
  float ambient = 0.2f;

  float specularLight = 0.5f;
  vec3 viewDirection = normalize(u_camPos - vertPos);
  vec3 reflectionDirection = reflect(-lightDirection, normal);
  float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.f), 8);
  float specular = specAmount * specularLight;

  return vec4(color, 1.f) * (diffuse + ambient + specular) * vec4(u_lightColor, 1.f);
}

void main() {
  FragColor = directionalLight();
}

