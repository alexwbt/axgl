#version 420 core

uniform sampler2D screen;

in vec2 vert_uv;

out vec4 frag_color;

void main()
{
  frag_color = texture(screen, vert_uv);
}
