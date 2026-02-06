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
  vec4 light_space_position;
} vs_out;

void main()
{
  gl_Position = projection_view * model * vec4(position, 1.0);
  gl_Position.x = -gl_Position.x;

  vs_out.position = vec3(model * vec4(position, 1.0));
  vs_out.normal = normalize(mat3(transpose(inverse(model))) * normal);
  vs_out.uv = uv;

  vs_out.light_space_position = light_pv * vec4(vs_out.position, 1.0);
}
