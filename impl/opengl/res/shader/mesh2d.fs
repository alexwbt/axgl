#version 330 core

uniform vec3 mesh_color;
uniform bool use_texture;
uniform sampler2D mesh_texture;

out vec4 FragColor;

in vec2 vert_uv;

void main()
{
  vec4 color = vec4(mesh_color, 1.0);
  FragColor = use_texture ? texture(mesh_texture, vert_uv) * color : color;
}
