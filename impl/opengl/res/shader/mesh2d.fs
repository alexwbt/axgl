#version 420 core

uniform vec4 mesh_color;
uniform bool use_texture;
uniform sampler2D mesh_texture;

in vec2 vert_uv;

out vec4 frag_color;

void main()
{
  frag_color = use_texture
    ? texture(mesh_texture, vert_uv) * mesh_color
    : mesh_color;

  if (frag_color.a < 0.01)
    discard;
}
