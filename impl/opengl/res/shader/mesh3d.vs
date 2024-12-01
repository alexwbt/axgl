#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 vert_position;
out vec3 vert_normal;
out vec2 vert_uv;

uniform mat4 mvp;
uniform mat4 model;

void main()
{
  gl_Position = mvp * vec4(position, 1.0);

  vert_position = vec3(model * vec4(position, 1.0));
  vert_normal = normalize(mat3(transpose(inverse(model))) * normal);
  vert_uv = uv;
}
