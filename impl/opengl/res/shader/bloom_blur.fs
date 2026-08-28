#version 410 core

uniform sampler2D source_texture;
uniform vec2 texel_size;
uniform vec2 direction;

in vec2 vert_uv;

out vec4 frag_color;

void main()
{
  float weights[5]
    = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

  vec3 result = texture(source_texture, vert_uv).rgb * weights[0];
  for (int i = 1; i < 5; ++i)
  {
    vec2 offset = direction * texel_size * float(i);
    result += texture(source_texture, vert_uv + offset).rgb * weights[i];
    result += texture(source_texture, vert_uv - offset).rgb * weights[i];
  }
  frag_color = vec4(result, 1.0);
}
