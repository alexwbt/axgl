#version 420 core

uniform vec4 text_color;
uniform sampler2D text_texture;

out vec4 FragColor;

in vec2 vert_uv;

void main()
{
  FragColor = text_color * vec4(1.0, 1.0, 1.0, texture(text_texture, vert_uv).r);
}
