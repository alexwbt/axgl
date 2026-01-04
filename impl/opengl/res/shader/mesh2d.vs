#version 410 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in mat4 instance_model;

uniform mat4 projection_view;
uniform mat4 projection_view_model;
uniform bool use_instancing;

out vec2 vert_uv;

void main()
{
  mat4 mvp = use_instancing ? projection_view * instance_model : projection_view_model;
  gl_Position = mvp * vec4(position, 0.0, 1.0);
  gl_Position.x = -gl_Position.x;

  vert_uv = vec2(uv.x, 1 - uv.y);
}
