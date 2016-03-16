#version 300 es
precision mediump float;

in vec4 vColour;
in vec3 vNormal;

out vec4 colour;

void main(void) {
  colour = vColour + vec4(vNormal, 1.0) * 0.5;
}
