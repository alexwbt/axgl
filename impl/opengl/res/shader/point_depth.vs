#version 410 core

layout(location = 0) in vec3 position;
layout(location = 5) in mat4 model;

uniform mat4 projection_view;

out vec3 world_position;

void main()
{
  vec4 world_pos = model * vec4(position, 1.0);
  world_position = world_pos.xyz;
  gl_Position = projection_view * world_pos;
}
