#version 410 core

uniform vec4 color;
uniform float opacity;
uniform bool use_texture;
uniform sampler2D background_texture;

in vec2 vert_uv;

out vec4 frag_color;

void main()
{
  frag_color = use_texture
    ? texture(background_texture, vert_uv)
    : color;
  frag_color.a *= opacity;
}
