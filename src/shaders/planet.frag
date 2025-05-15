#version 460 core

#define HEIGHT_MIN -10894
#define HEIGHT_MAX 7891
#define HEIGHT_METRES(h) (HEIGHT_MIN + (HEIGHT_MAX - HEIGHT_MIN) * h)

out vec4 FragColor;

in vec3 vertPos;
in vec2 texCoord;
in vec3 lightPos;
in float u0;
in float u1;
in float idx;

uniform vec3 u_lightColor;
uniform sampler2DArray u_normalheightmaps;
uniform sampler2DArray u_worldColors;

vec2 texUV = vec2((fwidth(u0) < fwidth(u1) - 0.001f) ? u0 : u1, texCoord.y);
vec3 normal = normalize(texture(u_normalheightmaps, vec3(texUV, idx)).rgb * 2.f - 1.f);
vec3 color = texture(u_worldColors, vec3(texUV, idx)).rgb;

void main() {
  vec3 lightDir = normalize(lightPos - vertPos);
  color *= max(0.f, dot(normal, lightDir)) * u_lightColor;

  FragColor = vec4(color, 1.f);
}

