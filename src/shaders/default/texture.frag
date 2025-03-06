#version 460 core

in vec3 color;
in vec2 tex;

out vec4 FragColor;

uniform sampler2D diffuse0;

void main() { FragColor = vec4(vec3(texture(diffuse0, tex).r), 1.f); }
