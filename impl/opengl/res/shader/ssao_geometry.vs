#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 5) in mat4 model;

uniform mat4 projection_view;
uniform mat4 view_matrix;

out VertexShaderOutput
{
  vec3 view_position;
  vec3 view_normal;
}
vso;

void main()
{
  vec4 world_pos = model * vec4(position, 1.0);
  gl_Position = projection_view * world_pos;
  gl_Position.x = -gl_Position.x;

  vso.view_position = vec3(view_matrix * world_pos);

  mat3 normal_matrix = transpose(inverse(mat3(model)));
  vec3 world_normal = normalize(normal_matrix * normal);
  vso.view_normal = normalize(mat3(view_matrix) * world_normal);
}
