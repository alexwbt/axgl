#version 420 core

uniform vec4 text_color;
uniform sampler2D text_texture;

in vec2 vert_uv;

out vec4 frag_color;

void main()
{
  frag_color = text_color * vec4(1.0, 1.0, 1.0, texture(text_texture, vert_uv).r);
}
