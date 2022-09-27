#version 330 core
out vec4 FragColor;

vec4 EncodeFloatRGBA(float v) {
   return fract( vec4(1.0, 255.0, 65025.0, 16581375.0) * v ) + 0.5/255.0;
 }

float DecodeFloatRGBA(vec4 rgba) {
   return dot( rgba, vec4(1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0) );
 }

void main (void) {
  FragColor = EncodeFloatRGBA(gl_FragCoord.z);
}