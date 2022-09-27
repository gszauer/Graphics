#version 330 core
in  vec2 TexCoords;
  
uniform sampler2D fboAttachment;
out vec4 FragColor;
  
void main() {
    float depth = texture(fboAttachment, TexCoords).r;
    FragColor = vec4(depth, depth, depth, 1.0);
} 