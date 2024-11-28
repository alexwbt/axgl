#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec3 vert_position;
out vec3 vert_normal;

uniform mat4 mvp;
uniform mat4 model;

void main()
{
  gl_Position = mvp * vec4(pos, 1.0);

  vert_position = vec3(model * vec4(pos, 1.0));
  vert_normal = normalize(mat3(transpose(inverse(model))) * normal);
}
