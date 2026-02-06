#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bitangent;
layout (location = 4) in vec2 uv;
layout (location = 5) in mat4 model;

uniform mat4 projection_view;
uniform mat4 light_pv;

out VertexShaderOutput {
  vec3 position;
  vec3 normal;
  vec2 uv;
  mat3 tbn;
  vec3 tbn_position;
  vec4 light_space_position;
} vso;

void main()
{
  gl_Position = projection_view * model * vec4(position, 1.0);
  gl_Position.x = -gl_Position.x;

  vso.position = vec3(model * vec4(position, 1.0));
  vso.normal = normalize(mat3(transpose(inverse(model))) * normal);
  vso.uv = uv;

  // shadow map test
  vso.light_space_position = light_pv * vec4(vso.position, 1.0);

  // TBN
  mat3 normal_matrix = transpose(inverse(mat3(model)));
  vec3 T = normalize(normal_matrix * tangent);
  vec3 N = normalize(normal_matrix * normal);
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);
  vso.tbn = transpose(mat3(T, B, N));
  vso.tbn_position = vso.tbn * vso.position;
}
