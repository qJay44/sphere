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
uniform sampler2DArray u_normalheightmaps;
uniform sampler2DArray u_worldColors;
uniform sampler2DArray u_borders;
uniform float u_lightMultiplier;

vec2 texUV = vec2((fwidth(u0) < fwidth(u1) - 0.001f) ? u0 : u1, texCoord.y);
vec3 normal = texture(u_normalheightmaps, vec3(texUV, idx)).rgb * 2.f - 1.f;
vec3 color = texture(u_worldColors, vec3(texUV, idx)).rgb;
vec3 border = vec3(texture(u_borders, vec3(texUV, idx)).r);

void main() {
  vec3 lightDirection = normalize(lightPos - vertPos);
  color *= max(0.f, dot(normal, lightDirection)) * u_lightColor * u_lightMultiplier;
  color += border;

  FragColor = vec4(color, 1.f);
}

