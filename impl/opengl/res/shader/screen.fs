#version 410 core

uniform sampler2D screen;
uniform bool enable_hdr;
uniform float exposure;

in vec2 vert_uv;

out vec4 frag_color;

void main()
{
  vec3 color = texture(screen, vert_uv).rgb;

  if (enable_hdr)
  {
    const float gamma = 2.2;
    vec3 mapped = vec3(1.0) - exp(-color * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));
    frag_color = vec4(mapped, 1.0);
  }
  else { frag_color = vec4(color, 1.0); }
}
