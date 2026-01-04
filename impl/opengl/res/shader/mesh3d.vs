#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in mat4 instance_model;

uniform mat4 projection_view;
uniform mat4 projection_view_model;
uniform bool use_instancing;

out vec3 vert_position;
out vec3 vert_normal;
out vec2 vert_uv;

void main()
{
  mat4 model = use_instancing ? instance_model : projection_view_model;
  gl_Position = projection_view * model * vec4(position, 1.0);
  gl_Position.x = -gl_Position.x;

  vert_position = vec3(model * vec4(position, 1.0));
  vert_normal = normalize(mat3(transpose(inverse(model))) * normal);
  vert_uv = uv;
}
