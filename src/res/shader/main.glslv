#version 330
precision mediump float;

uniform mat4  mvp;

in vec4 position;
in vec3 normal;
in vec4 colour;

out vec4 vColour;
out vec3 vNormal;

void main(void) {
  vColour = colour;
  vNormal = normal;
  gl_Position = position;
}
