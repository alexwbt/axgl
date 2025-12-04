#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in mat4 model;

out vec3 vert_position;
out vec3 vert_normal;
out vec2 vert_uv;

uniform mat4 projection_view;
uniform mat4 projection_view_model;
uniform bool use_instancing;

void main()
{
  mat4 mvp = use_instancing ? projection_view * model : projection_view_model;
  gl_Position = mvp * vec4(position, 1.0);
  gl_Position.x = -gl_Position.x;

  vert_position = vec3(model * vec4(position, 1.0));
  vert_normal = normalize(mat3(transpose(inverse(model))) * normal);
  vert_uv = uv;
}
