#version 420 core

uniform vec4 color;
uniform float opacity;

out vec4 frag_color;

void main()
{
  frag_color = color;
  frag_color.a *= opacity;
}
