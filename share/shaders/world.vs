#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 vertexColor;

out vec4 fragmentColor;

uniform mat4 vMVP;

void main() {
  vec4 pos = vec4(position, 1.0f);
  fragmentColor = vertexColor;
  gl_Position = vMVP * pos;
}

