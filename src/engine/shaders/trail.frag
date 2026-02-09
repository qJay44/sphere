#version 460 core

in float alpha;
in vec2 texCoord;

out vec4 FragColor;

void main() {
	FragColor = vec4(vec3(1.f), alpha);
}

