#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uColorSpec;

void main() {
	FragColor = vec4(texture(uColorSpec, TexCoord).rgb, 1.0);
}