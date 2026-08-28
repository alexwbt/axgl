#version 410 core

uniform sampler2D screen_texture;
uniform float threshold;

in vec2 vert_uv;

out vec4 frag_color;

void main()
{
  vec3 color = texture(screen_texture, vert_uv).rgb;
  float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
  float contribution = clamp(luminance - threshold, 0.0, 1.0);
  contribution /= max(luminance, 0.00001);
  frag_color = vec4(color * contribution, 1.0);
}
