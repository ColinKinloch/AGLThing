#version 300 es
precision mediump float;

uniform mat4 mvp;
uniform mat3 normalMatrix;

in vec4 position;
in vec3 normal;
in vec4 colour;

out vec4 vColour;
out vec3 vNormal;

void main(void) {
  vColour = colour;
  vNormal = normal * normalMatrix;
  gl_Position = position;
}
