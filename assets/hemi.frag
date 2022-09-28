#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in mat3 TBN;

out vec4 FragColor;

uniform sampler2D uColorSpec;
uniform sampler2D uNormal;

uniform mat4 model;
uniform vec3 HemiTop;
uniform vec3 HemiBottom;

uniform vec3 LightDirection;
uniform vec3 LightColor;
uniform float AmbientOnly;

uniform vec3 ViewPos;

void main() {
	vec3 normal = texture(uNormal, TexCoord).rgb; 
	normal = normal * 2.0 - 1.0;
	normal = normalize(TBN * normal); // World space normal

	vec4 colorSpec = texture(uColorSpec, TexCoord);
	vec3 color = colorSpec.rgb;

	vec3 lightDir = normalize(-LightDirection);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * LightColor;

	diff = max(dot(normal, normalize(vec3(-0.25, -1, 1))), 0.0);
	vec3 ambient = mix(HemiBottom, HemiTop, diff)* LightColor; // Color comes from hemi, doesn't need to multiply

	vec3 viewDir = normalize(ViewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	vec3 specular = colorSpec.a * spec * LightColor;  

	vec4 comp0 = vec4((ambient + diffuse + specular) * color, 1.0);
	vec4 comp1 = vec4(ambient, 1.0);
	FragColor =  mix(comp0, comp1, AmbientOnly);
}