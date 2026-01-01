#version 420 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

uniform mat4 projection_view_model;

out vec2 vert_uv;

void main()
{
  gl_Position = projection_view_model * vec4(position, 0.0, 1.0);
  gl_Position.x = -gl_Position.x;

  vert_uv = uv;
}
