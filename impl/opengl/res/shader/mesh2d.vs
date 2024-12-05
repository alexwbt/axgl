#version 330 core

layout (location = 0) in vec2 position;
layout (location = 2) in vec2 uv;

uniform mat4 mvp;

out vec2 vert_uv;

void main()
{
  gl_Position = mvp * vec4(position, 0.0, 1.0);
  vert_uv = uv;
}
