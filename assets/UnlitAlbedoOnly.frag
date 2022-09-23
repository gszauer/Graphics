#version 330 core

in vec2 TexCoord;

out vec4 FragColor;
uniform sampler2D uColorSpec;

void main() {
	FragColor = vec4(texture(uColorSpec, TexCoord).rgb, 1.0);
}