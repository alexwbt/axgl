#version 410 core

uniform sampler2D ssao_texture;

in vec2 vert_uv;

out vec4 frag_color;

void main()
{
  vec2 texel_size = 1.0 / textureSize(ssao_texture, 0);
  float result = 0.0;
  for (int x = -2; x <= 2; ++x)
  {
    for (int y = -2; y <= 2; ++y)
    {
      vec2 offset = vec2(float(x), float(y)) * texel_size;
      result += texture(ssao_texture, vert_uv + offset).r;
    }
  }
  result /= 25.0;
  frag_color = vec4(vec3(result), 1.0);
}
