#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 vert_uv;

void main()
{
  gl_Position = vec4(position * 2.0f - 1.0f, 1.0f);
  vert_uv = uv;
}
