#version 330 core
out vec4 FragColor;
in  vec2 TexCoords;
  
uniform sampler2D fboAttachment;
  
void main() {
    float depth = texture(fboAttachment, TexCoords).r;
    FragColor = vec4(depth, depth, depth, 1.0);
} 