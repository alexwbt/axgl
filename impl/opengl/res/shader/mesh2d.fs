#version 410 core

uniform vec4 mesh_color;
uniform bool use_texture;
uniform sampler2D mesh_texture;
uniform float alpha_discard;
uniform float texture_gamma;

in vec2 vert_uv;

out vec4 frag_color;


vec4 texture_color()
{
  vec4 value = texture(mesh_texture, vert_uv);
  return vec4(pow(value.rgb, vec3(texture_gamma)), value.a);
}

void main()
{
  frag_color = use_texture
    ? texture_color() * mesh_color
    : mesh_color;

  if (frag_color.a < alpha_discard)
    discard;
}
