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

struct LightingContext
{
  vec3 view_dir;
  vec3 frag_diffuse;
  vec3 frag_specular;
  vec3 frag_normal;
};

uniform vec3 camera_pos;
uniform vec4 mesh_color;
uniform float mesh_shininess;
uniform float mesh_specular;

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
uniform sampler2D shadow_map;

in VertexShaderOutput {
  vec3 position;
  vec3 normal;
  vec2 uv;
  mat3 tbn;
  vec3 tbn_position;
  vec4 light_space_position;
} vso;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out float reveal;

vec2 get_uv()
{
  return (vso.uv + uv_offset) * uv_scale;
}

vec3 get_frag_diffuse()
{
  return use_diffuse_texture
    ? pow(texture(diffuse_texture, get_uv()).rgb, vec3(diffuse_texture_gamma)) * mesh_color.rgb
    : mesh_color.rgb;
}

vec3 get_frag_specular()
{
  return use_specular_texture
    ? texture(specular_texture, get_uv()).rgb * mesh_specular
    : vec3(mesh_specular);
}

vec3 get_frag_normal()
{
  if (!use_normal_texture)
    return vec3(vso.normal);

  vec3 normal_value = texture(normal_texture, get_uv()).rgb;
  normal_value = normalize(normal_value * 2.0 - 1.0);
  return normal_value;
}

float calc_shadow()
{
  vec3 projection_coords = vso.light_space_position.xyz / vso.light_space_position.w;
  projection_coords = projection_coords * 0.5 + 0.5;

  if (projection_coords.z > 1.0) return 0.0;

//  float closest_depth = texture(shadow_map, projection_coords.xy).r;
//  float current_depth = projection_coords.z;
//  float bias = max(0.05 * (1.0 - dot(vso.normal, light_dir)), 0.005);
//  float shadow = current_depth - bias > closest_depth  ? 1.0 : 0.0;
//  float shadow = current_depth > closest_depth  ? 1.0 : 0.0;
  float shadow = 0.0;
  vec2 texel_size = 1.0 / textureSize(shadow_map, 0);
  for(int x = -1; x <= 1; ++x)
  {
    for(int y = -1; y <= 1; ++y)
    {
      float pcf_depth = texture(shadow_map, projection_coords.xy + vec2(x, y) * texel_size).r;
      shadow += projection_coords.z > pcf_depth ? 1.0 : 0.0;
    }
  }
  shadow /= 9.0;

  return shadow;
}

vec3 calc_sun_light(LightingContext ctx, SunLight light)
{
  // Diffuse
  vec3 light_dir = normalize(vso.tbn * -light.direction);
  float diffuse_value = max(dot(ctx.frag_normal, light_dir), 0.0);
  vec3 diffuse = light.diffuse * diffuse_value * ctx.frag_diffuse;

  // Specular
  vec3 reflect_dir = reflect(-light_dir, ctx.frag_normal);
  vec3 specular = diffuse_value == 0.0 ? vec3(0.0) : light.specular
    * pow(max(dot(ctx.view_dir, reflect_dir), 0.0), mesh_shininess)
    * ctx.frag_specular;

  // Ambient
  vec3 ambient = light.ambient * ctx.frag_diffuse;

  // Shadow
  float shadow = calc_shadow();

  return ambient + (1.0 - shadow) * (diffuse + specular);
}

vec3 calc_spot_light(LightingContext ctx, SpotLight light)
{
  // Diffuse
  vec3 light_dir = normalize(light.position - vso.position);
  float diffuse_value = max(dot(ctx.frag_normal, light_dir), 0.0);
  vec3 diffuse = light.diffuse * diffuse_value * ctx.frag_diffuse;

  // Specular
  vec3 reflect_dir = reflect(-light_dir, ctx.frag_normal);
  vec3 specular = diffuse_value == 0.0 ? vec3(0.0) : light.specular
    * pow(max(dot(ctx.view_dir, reflect_dir), 0.0), mesh_shininess)
    * ctx.frag_specular;

  // Ambient
  vec3 ambient = light.ambient * ctx.frag_diffuse;

  // Attenuation
  float dis = length(light.position - vso.position);
  float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * (dis * dis));

  // Cut Off
  float theta = dot(light_dir, normalize(-light.direction));
  float epsilon = light.cut_off - light.outer_cut_off;
  float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

  return (ambient + (diffuse + specular) * intensity) * attenuation;
}

vec3 calc_point_light(LightingContext ctx, PointLight light)
{
  // Diffuse
  vec3 light_dir = normalize(light.position - vso.position);
  float diffuse_value = max(dot(ctx.frag_normal, light_dir), 0.0);
  vec3 diffuse = light.diffuse * diffuse_value * ctx.frag_diffuse;

  // Specular
  vec3 reflect_dir = reflect(-light_dir, ctx.frag_normal);
  vec3 specular = diffuse_value == 0.0 ? vec3(0.0) : light.specular
    * pow(max(dot(ctx.view_dir, reflect_dir), 0.0), mesh_shininess)
    * ctx.frag_specular;

  // Ambient
  vec3 ambient = light.ambient * ctx.frag_diffuse;

  // Attenuation
  float dis = length(light.position - vso.position);
  float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * (dis * dis));

  return (ambient + diffuse + specular) * attenuation;
}

void main()
{
  if (mesh_color.a < alpha_discard)
    discard;

  LightingContext ctx;
  ctx.view_dir = normalize((vso.tbn * camera_pos) - vso.tbn_position);
  ctx.frag_diffuse = get_frag_diffuse();
  ctx.frag_specular = get_frag_specular();
  ctx.frag_normal = get_frag_normal();

  vec3 result = vec3(0.0);

  // sun lights
  for (int i = 0; i < sun_lights_size; ++i)
    result += calc_sun_light(ctx, sun_lights[i]);
  // spot lights
  for (int i = 0; i < spot_lights_size; ++i)
    result += calc_spot_light(ctx, spot_lights[i]);
  // point lights
  for (int i = 0; i < point_lights_size; ++i)
    result += calc_point_light(ctx, point_lights[i]);

  // weight function for blending
  float weight = transparent
    ? clamp(pow(min(1.0, mesh_color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3)
    : 1.0f;
  frag_color = vec4(result, mesh_color.a) * weight;
  reveal = mesh_color.a;
}
