#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bitangent;
layout (location = 4) in vec2 uv;
layout (location = 5) in mat4 model;

uniform vec3 camera_pos;
uniform mat4 projection_view;
uniform mat4 light_pv;
uniform vec2 uv_scale;
uniform vec2 uv_offset;
uniform bool use_normal_texture;

out VertexShaderOutput {
  vec3 camera_pos;
  vec3 position;
  vec3 normal;
  vec2 uv;
  mat3 tbn;
  vec4 light_space_position;
} vso;

void main()
{
  vec4 position_v4 = vec4(position, 1.0);
  gl_Position = projection_view * model * position_v4;
  gl_Position.x = -gl_Position.x;

  mat3 normal_matrix = mat3(model);

  vso.camera_pos = camera_pos;
  vso.position = vec3(model * position_v4);
  vso.normal = normalize(normal_matrix * normal);
  vso.uv = (uv + uv_offset) * uv_scale;

  vso.light_space_position = light_pv * vec4(vso.position, 1.0);

  if (use_normal_texture)
  {
    vec3 t = normalize(normal_matrix * tangent);
    vec3 b = normalize(normal_matrix * bitangent);
    vso.tbn = transpose(mat3(t, b, vso.normal));
    vso.position = vso.tbn * vso.position;
    vso.camera_pos = vso.tbn * vso.camera_pos;
  }
  else
  {
    vso.tbn = mat3(1.0);
  }
}
