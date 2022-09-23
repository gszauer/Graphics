#version 330 core

in vec3 aPos;
in vec3 aNorm;
in vec3 aTan;
in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out mat3 TBN;
out vec2 TexCoord;
out vec3 FragPos;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));
	
	vec3 biTangent = cross(aNorm, aTan);
	vec3 T = normalize(vec3(model * vec4(aTan,   0.0)));
	vec3 B = normalize(vec3(model * vec4(biTangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNorm,    0.0)));
	TBN = mat3(T, B, N);

	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}