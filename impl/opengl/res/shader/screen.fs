#version 410 core

uniform sampler2D screen;

in vec2 vert_uv;

out vec4 frag_color;

void main()
{
  vec4 color = texture(screen, vert_uv);
  // Reinhard tone mapping
  color.rgb = color.rgb / (color.rgb + vec3(1.0));
  frag_color = vec4(color.rgb, 1.0);
}
