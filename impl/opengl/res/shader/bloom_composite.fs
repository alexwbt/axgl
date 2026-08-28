#version 410 core

uniform sampler2D screen_texture;
uniform sampler2D bloom_texture;
uniform float intensity;

in vec2 vert_uv;

out vec4 frag_color;

void main()
{
  vec3 color = texture(screen_texture, vert_uv).rgb;
  vec3 bloom = texture(bloom_texture, vert_uv).rgb;
  frag_color = vec4(color + bloom * intensity, 1.0);
}
