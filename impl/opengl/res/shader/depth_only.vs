#version 410 core

layout (location = 0) in vec3 position;
layout (location = 3) in mat4 model;

uniform mat4 projection_view;

void main()
{
  gl_Position = projection_view * model * vec4(position, 1.0);
  gl_Position.x = -gl_Position.x;
}
