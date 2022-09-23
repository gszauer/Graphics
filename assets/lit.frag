#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in mat3 TBN;
in vec4 LightViewPos;

out vec4 FragColor;

uniform sampler2D uColorSpec;
uniform sampler2D uNormal;
uniform sampler2D uShadowMap; 

uniform mat4 model;

uniform vec3 LightDirection;
uniform vec3 LightColor;
uniform float AmbientStrength;

uniform vec3 ViewPos;

uniform float AmbientOnly;

void main() {
	vec3 normal = texture(uNormal, TexCoord).rgb; 
	normal = normal * 2.0 - 1.0;
	normal = normalize(TBN * normal); // World space normal

	vec4 colorSpec = texture(uColorSpec, TexCoord);
	vec3 objectColor = colorSpec.rgb;

	vec3 lightDir = normalize(-LightDirection);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * LightColor;

	vec3 ambient = AmbientStrength * LightColor;

	vec3 viewDir = normalize(ViewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = colorSpec.a * spec * LightColor;  

	vec4 comp0 = vec4((ambient+diffuse+specular) * objectColor, 1.0);
	vec4 comp1 = vec4(ambient, 1.0);
	vec4 FinalColor = mix(comp0, comp1, AmbientOnly);

	vec3 p = LightViewPos.xyz / LightViewPos.w;
	FragColor = FinalColor * (texture(uShadowMap, p.xy).r < p.z? 0 : 1);
}