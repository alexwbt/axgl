#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in mat4 model;

uniform mat4 projection_view;

out vec3 frag_position;
out vec3 frag_normal;
out vec2 vert_uv;

void main()
{
  gl_Position = projection_view * model * vec4(position, 1.0);
  gl_Position.x = -gl_Position.x;

  frag_position = vec3(model * vec4(position, 1.0));
  frag_normal = normalize(mat3(transpose(inverse(model))) * normal);
  vert_uv = uv;
}
