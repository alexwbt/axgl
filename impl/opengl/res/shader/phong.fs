#version 410 core

struct SunLight
{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct SpotLight
{
  vec3 position;
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;

  float cut_off;
  float outer_cut_off;
};

struct PointLight
{
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

uniform vec3 camera_pos;
uniform vec4 mesh_color;
uniform float mesh_gloss;
uniform sampler2D diffuse_texture;
uniform bool use_diffuse_texture;
uniform sampler2D specular_texture;
uniform bool use_specular_texture;
uniform sampler2D normal_texture;
uniform bool use_normal_texture;
uniform sampler2D height_texture;
uniform bool use_height_texture;
uniform int sun_lights_size;
uniform SunLight sun_lights[32];
uniform int spot_lights_size;
uniform SpotLight spot_lights[32];
uniform int point_lights_size;
uniform PointLight point_lights[32];
uniform bool transparent;
uniform float alpha_discard;
uniform float max_shininess;
uniform vec2 uv_scale;
uniform vec2 uv_offset;

in vec3 vert_position;
in vec3 vert_normal;
in vec2 vert_uv;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out float reveal;

vec3 get_mesh_diffuse()
{
  return use_diffuse_texture
    ? texture(diffuse_texture, (vert_uv + uv_offset) * uv_scale).rgb
    : mesh_color.rgb;
}

vec3 get_mesh_specular()
{
  return use_specular_texture
    ? texture(specular_texture, (vert_uv + uv_offset) * uv_scale).rgb
    : vec3(1);
}

vec3 calc_sun_light(SunLight light, vec3 view_dir)
{
  vec3 light_dir = normalize(-light.direction);

  vec3 mesh_diffuse = get_mesh_diffuse();
  vec3 mesh_specular = get_mesh_specular();

  vec3 ambient = light.ambient * mesh_diffuse;

  // Diffuse
  vec3 diffuse = light.diffuse * max(dot(vert_normal, light_dir), 0.0) * mesh_diffuse;
  // Specular
  vec3 reflect_dir = reflect(-light_dir, vert_normal);
  vec3 specular = light.specular
    * pow(max(dot(view_dir, reflect_dir), 0.0), max(mesh_gloss * max_shininess, 1.0f))
    * mesh_specular;

  return ambient + diffuse + specular;
}

vec3 calc_spot_light(SpotLight light, vec3 view_dir)
{
  vec3 light_dir = normalize(light.position - vert_position);

  vec3 mesh_diffuse = get_mesh_diffuse();
  vec3 mesh_specular = get_mesh_specular();

  vec3 ambient = light.ambient * mesh_diffuse;

  // Diffuse
  vec3 diffuse = light.diffuse * max(dot(vert_normal, light_dir), 0.0) * mesh_diffuse;

  // Specular
  vec3 reflect_dir = reflect(-light_dir, vert_normal);
  vec3 specular = light.specular
    * pow(max(dot(view_dir, reflect_dir), 0.0), max(mesh_gloss * max_shininess, 1.0f))
    * mesh_specular;

  // Attenuation
  float dis = length(light.position - vert_position);
  float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * (dis * dis));

  // Cut Off
  float theta = dot(light_dir, normalize(-light.direction));
  float epsilon = light.cut_off - light.outer_cut_off;
  float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

  return (ambient + (diffuse + specular) * intensity) * attenuation;
}

vec3 calc_point_light(PointLight light, vec3 view_dir)
{
  vec3 light_dir = normalize(light.position - vert_position);

  vec3 mesh_diffuse = get_mesh_diffuse();
  vec3 mesh_specular = get_mesh_specular();

  vec3 ambient = light.ambient * mesh_diffuse;

  // Diffuse
  vec3 diffuse = light.diffuse * max(dot(vert_normal, light_dir), 0.0) * mesh_diffuse;

  // Specular
  vec3 reflect_dir = reflect(-light_dir, vert_normal);
  vec3 specular = light.specular
    * pow(max(dot(view_dir, reflect_dir), 0.0), max(mesh_gloss * max_shininess, 1.0f))
    * mesh_specular;

  // Attenuation
  float dis = length(light.position - vert_position);
  float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * (dis * dis));

  return (ambient + diffuse + specular) * attenuation;
}

void main()
{
  if (mesh_color.a < alpha_discard)
    discard;

  vec3 view_dir = normalize(camera_pos - vert_position);
  vec3 result = vec3(0);

  // sun lights
  for (int i = 0; i < sun_lights_size; ++i)
    result += calc_sun_light(sun_lights[i], view_dir);
  // spot lights
  for (int i = 0; i < spot_lights_size; ++i)
    result += calc_spot_light(spot_lights[i], view_dir);
  // point lights
  for (int i = 0; i < point_lights_size; ++i)
    result += calc_point_light(point_lights[i], view_dir);

  // weight function for blending
  float weight = transparent
    ? clamp(pow(min(1.0, mesh_color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3)
    : 1.0f;
  frag_color = vec4(result, mesh_color.a) * weight;
  reveal = mesh_color.a;
}
