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
uniform float shininess;
uniform sampler2D diffuse_texture;
uniform float diffuse_texture_gamma;
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
uniform vec2 uv_scale;
uniform vec2 uv_offset;

in vec3 frag_position;
in vec3 frag_normal;
in vec2 vert_uv;
in vec4 light_space_frag_pos;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out float reveal;

vec3 get_mesh_diffuse()
{
  return use_diffuse_texture
    ? pow(texture(diffuse_texture, (vert_uv + uv_offset) * uv_scale).rgb, vec3(diffuse_texture_gamma))
    : mesh_color.rgb;
}

vec3 get_mesh_specular()
{
  return use_specular_texture
    ? texture(specular_texture, (vert_uv + uv_offset) * uv_scale).rgb
    : vec3(1);
}

float calc_shadow()
{
  // perform perspective divide
  vec3 projection_coords = light_space_frag_pos.xyz / light_space_frag_pos.w;

  // transform to [0,1] range
  projection_coords = projection_coords * 0.5 + 0.5;

  // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
  float closest_depth = texture(shadowMap, projection_coords.xy).r;

  // get depth of current fragment from light's perspective
  float current_depth = projection_coords.z;

  // check whether current frag pos is in shadow
  float shadow = current_depth > closest_depth  ? 1.0 : 0.0;

  return shadow;
}

vec3 calc_sun_light(SunLight light, vec3 view_dir)
{
  vec3 mesh_diffuse = get_mesh_diffuse();
  vec3 mesh_specular = get_mesh_specular();

  // Diffuse
  vec3 light_dir = normalize(-light.direction);
  float diffuse_value = max(dot(frag_normal, light_dir), 0.0);
  vec3 diffuse = light.diffuse * diffuse_value * mesh_diffuse;

  // Specular
  vec3 reflect_dir = reflect(-light_dir, frag_normal);
  vec3 specular = diffuse_value == 0.0 ? vec3(0.0) : light.specular
    * pow(max(dot(view_dir, reflect_dir), 0.0), shininess)
    * mesh_specular;

  // Ambient
  vec3 ambient = light.ambient * mesh_diffuse;

  // Shadow
  float shadow = calc_shadow();

  return ambient + (1.0 - shadow) * (diffuse + specular);
}

vec3 calc_spot_light(SpotLight light, vec3 view_dir)
{
  vec3 mesh_diffuse = get_mesh_diffuse();
  vec3 mesh_specular = get_mesh_specular();

  // Diffuse
  vec3 light_dir = normalize(light.position - frag_position);
  float diffuse_value = max(dot(frag_normal, light_dir), 0.0);
  vec3 diffuse = light.diffuse * diffuse_value * mesh_diffuse;

  // Specular
  vec3 reflect_dir = reflect(-light_dir, frag_normal);
  vec3 specular = diffuse_value == 0.0 ? vec3(0.0) : light.specular
    * pow(max(dot(view_dir, reflect_dir), 0.0), shininess)
    * mesh_specular;

  // Ambient
  vec3 ambient = light.ambient * mesh_diffuse;

  // Attenuation
  float dis = length(light.position - frag_position);
  float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * (dis * dis));

  // Cut Off
  float theta = dot(light_dir, normalize(-light.direction));
  float epsilon = light.cut_off - light.outer_cut_off;
  float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

  return (ambient + (diffuse + specular) * intensity) * attenuation;
}

vec3 calc_point_light(PointLight light, vec3 view_dir)
{
  vec3 mesh_diffuse = get_mesh_diffuse();
  vec3 mesh_specular = get_mesh_specular();

  // Diffuse
  vec3 light_dir = normalize(light.position - frag_position);
  float diffuse_value = max(dot(frag_normal, light_dir), 0.0);
  vec3 diffuse = light.diffuse * diffuse_value * mesh_diffuse;

  // Specular
  vec3 reflect_dir = reflect(-light_dir, frag_normal);
  vec3 specular = diffuse_value == 0.0 ? vec3(0.0) : light.specular
    * pow(max(dot(view_dir, reflect_dir), 0.0), shininess)
    * mesh_specular;

  // Ambient
  vec3 ambient = light.ambient * mesh_diffuse;

  // Attenuation
  float dis = length(light.position - frag_position);
  float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * (dis * dis));

  return (ambient + diffuse + specular) * attenuation;
}

void main()
{
  if (mesh_color.a < alpha_discard)
    discard;

  vec3 view_dir = normalize(camera_pos - frag_position);
  vec3 result = vec3(0.0);

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
