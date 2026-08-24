#version 410 core

layout(location = 0) in vec3 position;
layout(location = 5) in mat4 model;

uniform mat4 projection_view;

void main()
{
  vec4 position_v4 = vec4(position, 1.0);
  gl_Position = projection_view * model * position_v4;
  gl_Position.x = -gl_Position.x;
}
