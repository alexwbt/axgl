#version 410 core

uniform sampler2D position_texture;
uniform sampler2D normal_texture;
uniform sampler2D noise_texture;

uniform mat4 projection_matrix;
uniform vec2 noise_scale;
uniform float radius;
uniform float bias;

#define KERNEL_SIZE 64
uniform vec3 kernel_samples[KERNEL_SIZE];

in vec2 vert_uv;

out vec4 frag_color;

void main()
{
  vec3 frag_pos = texture(position_texture, vert_uv).xyz;
  vec3 normal = normalize(texture(normal_texture, vert_uv).xyz);
  vec3 random_vec = texture(noise_texture, vert_uv * noise_scale).xyz;

  vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 tbn = mat3(tangent, bitangent, normal);

  float occlusion = 0.0;
  for (int i = 0; i < KERNEL_SIZE; ++i)
  {
    vec3 sample_pos = tbn * kernel_samples[i];
    sample_pos = frag_pos + sample_pos * radius;

    vec4 offset = projection_matrix * vec4(sample_pos, 1.0);
    offset.x = -offset.x;
    offset.xyz /= offset.w;
    offset.xyz = offset.xyz * 0.5 + 0.5;

    float sample_depth = texture(position_texture, offset.xy).z;

    float range_check
      = smoothstep(0.0, 1.0, radius / abs(frag_pos.z - sample_depth));

    occlusion
      += (sample_pos.z + bias <= sample_depth ? 1.0 : 0.0) * range_check;
  }
  occlusion = 1.0 - (occlusion / float(KERNEL_SIZE));

  frag_color = vec4(vec3(occlusion), 1.0);
}
