#version 410 core
/**
 * mesh3d.fs - Fragment shader for 3D mesh rendering.
 *
 * Implements Blinn-Phong lighting with support for:
 *   - Diffuse/specular/normal/height textures
 *   - Parallax occlusion mapping (POM) for height textures
 *   - Normal mapping (tangent-space samples transformed to world space)
 *   - Sun, point, and spot lights (up to 32 each)
 *   - Cascaded shadow mapping with 3x3 PCF (sampler2DArray, per-cascade
 * selection)
 *   - Weighted blended transparency (output to MRT: color + reveal)
 *
 * Coordinate spaces:
 *   All lighting is computed in world space. The TBN matrix from the vertex
 *   shader (tangent->world) is used to transform normal-map samples to world
 *   space, and its transpose (world->tangent) is used only inside
 *   calc_height_offset to derive the tangent-space view direction for POM.
 *   Light positions/directions and the fragment position are all world space.
 *
 * Texture units:
 *   0: diffuse  (gamma-corrected via diffuse_texture_gamma)
 *   1: specular
 *   2: normal   (tangent-space, [0,1] -> [-1,1])
 *   3: height  (single-channel, sampled at base mip via textureLod)
 *   5: shadow maps (2D array, one layer per cascade)
 *
 * Outputs:
 *   location 0: frag_color - weighted, alpha-premultiplied color
 *   location 1: reveal     - per-pixel coverage for weighted blended OIT
 */

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

struct Context
{
  vec3 view_dir;
  vec3 frag_diffuse;
  vec3 frag_specular;
  vec3 frag_normal;
  float ssao;
};

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

uniform sampler2D ssao_texture;
uniform bool use_ssao;
uniform vec2 viewport;

uniform int sun_lights_size;
uniform SunLight sun_lights[SUN_LIGHT_LIMIT];

uniform int spot_lights_size;
uniform SpotLight spot_lights[SPOT_LIGHT_LIMIT];

uniform int point_lights_size;
uniform PointLight point_lights[POINT_LIGHT_LIMIT];

uniform bool transparent;
uniform float alpha_discard;
uniform float height_scale;
uniform float normal_scale;

uniform bool enable_sun_shadow;
uniform sampler2DArray sun_shadow_maps;
uniform float cascade_split_far[SUN_SHADOW_CASCADE_COUNT];
#ifdef AXGL_DEBUG
uniform bool csm_debug_borders;
#endif

in VertexShaderOutput
{
  vec3 camera_pos;
  vec3 position;
  vec3 normal;
  vec2 uv;
  mat3 tbn;
  vec4 light_space_position[SUN_SHADOW_CASCADE_COUNT];
}
vso;

layout(location = 0) out vec4 frag_color;
#ifndef OPAQUE_PASS
layout(location = 1) out float reveal;
#endif

/**
 * Parallax occlusion mapping: offsets texture coordinates based on the view
 * direction and a height map to simulate surface relief.
 *
 * The world-space view direction is converted to tangent space via the
 * transpose of vso.tbn (world->tangent). The ray is then marched through
 * depth layers; UVs are shifted opposite to the tangent-space view xy.
 *
 * view_dir.z is clamped to avoid unbounded UV displacement at grazing angles.
 * textureLod(..., 0) forces base-mip sampling to preserve height-field detail.
 */
vec2 calc_height_offset(Context ctx)
{
  // tangent-space view direction (TBN columns are t,b,n so transpose maps
  // world->tangent)
  vec3 view_dir_tangent = transpose(vso.tbn) * ctx.view_dir;

  // number of depth layers (min: 8, max: 32), more layers when viewing head-on
  float layers = mix(8.0, 32.0, abs(view_dir_tangent.z));
  // calculate the size of each layer
  float layer_depth = 1.0 / layers;
  // depth of current layer
  float current_layer_depth = 0.0;
  // the amount to shift the texture coordinates per layer (from vector P)
  // clamp view_dir.z to avoid unbounded displacement at grazing angles
  vec2 P
    = view_dir_tangent.xy / max(abs(view_dir_tangent.z), 0.1) * height_scale;
  vec2 delta_uv = P / layers;

  // initial values
  vec2 current_uv = vso.uv;
  float current_height_map_value
    = textureLod(height_texture, current_uv, 0.0).r;

  // march layers until the ray crosses the height-field surface
  while (current_layer_depth < current_height_map_value)
  {
    // shift texture coordinates along direction of P
    current_uv -= delta_uv;
    // get depth map value at current texture coordinates
    current_height_map_value = textureLod(height_texture, current_uv, 0.0).r;
    // get depth of next layer
    current_layer_depth += layer_depth;
  }

  // get texture coordinates before collision (reverse operations)
  vec2 prev_uv = current_uv + delta_uv;

  // get depth after and before collision for linear interpolation
  float after_depth = current_height_map_value - current_layer_depth;
  float before_depth = textureLod(height_texture, prev_uv, 0.0).r
    - current_layer_depth + layer_depth;

  // interpolation of texture coordinates between the two surrounding layers
  float weight = after_depth / (after_depth - before_depth);
  return prev_uv * weight + current_uv * (1.0 - weight);
}

/**
 * Selects the cascade index whose split range covers the fragment's distance
 * from the camera. Nearer cascades have higher depth precision.
 */
int select_cascade()
{
  float frag_distance = length(vso.position - vso.camera_pos);
  int cascade_index = 0;
  for (int i = 0; i < SUN_SHADOW_CASCADE_COUNT; ++i)
  {
    if (frag_distance <= cascade_split_far[i])
    {
      cascade_index = i;
      break;
    }
    cascade_index = i;
  }
  return cascade_index;
}

float calc_sun_shadow()
{
  int cascade_index = select_cascade();

  // project into the selected cascade's light clip space and remap to [0,1]
  vec3 projection_coords = vso.light_space_position[cascade_index].xyz
    / vso.light_space_position[cascade_index].w;
  projection_coords = projection_coords * 0.5 + 0.5;

  // beyond the light's far plane: not in shadow
  if (projection_coords.z > 1.0) return 0.0;

  // bias scales per cascade: farther cascades have lower depth precision and
  // need a larger bias to avoid self-shadowing acne.
  float bias = 0.00005 * (cascade_index + 1);
  float shadow = 0.0;
  // textureSize on a sampler2DArray returns ivec3(w, h, layers); .xy is the
  // per-layer texel size.
  vec2 texel_size = 1.0 / textureSize(sun_shadow_maps, 0).xy;
  for (int x = -1; x <= 1; ++x)
  {
    for (int y = -1; y <= 1; ++y)
    {
      // sample the array with vec3(uv, layer)
      float pcf_depth
        = texture(
            sun_shadow_maps,
            vec3(projection_coords.xy + vec2(x, y) * texel_size, cascade_index)
        )
            .r;
      shadow += projection_coords.z - bias > pcf_depth ? 1.0 : 0.0;
    }
  }
  shadow /= 9.0;

  return shadow;
}

/**
 * Sun (directional) light contribution.
 * light.direction points from the light toward the scene; negate for the
 * light-to-surface direction. Applies shadowing from the first light's
 * shadow map.
 */
vec3 calc_sun_light(Context ctx, SunLight light)
{
  // Diffuse
  vec3 light_dir = normalize(-light.direction);
  float diffuse_value = max(dot(ctx.frag_normal, light_dir), 0.0);
  vec3 diffuse = light.diffuse * diffuse_value * ctx.frag_diffuse;

  // Specular (skipped when the surface faces away or shininess is 0 to avoid
  // pow(0,0) NaN)
  vec3 reflect_dir = reflect(-light_dir, ctx.frag_normal);
  vec3 specular = (diffuse_value == 0.0 || mesh_shininess == 0.0)
    ? vec3(0.0)
    : light.specular
      * pow(max(dot(ctx.view_dir, reflect_dir), 0.0), mesh_shininess)
      * ctx.frag_specular;

  // Ambient
  vec3 ambient = light.ambient * ctx.frag_diffuse * ctx.ssao;

  // Shadow
  float shadow = enable_sun_shadow ? calc_sun_shadow() : 0.0;

  return ambient + (1.0 - shadow) * (diffuse + specular);
}

/**
 * Spot light contribution with distance attenuation and a soft cone cutoff.
 * light.direction points from the light toward the scene.
 */
vec3 calc_spot_light(Context ctx, SpotLight light)
{
  // Diffuse
  vec3 light_dir = normalize(light.position - vso.position);
  float diffuse_value = max(dot(ctx.frag_normal, light_dir), 0.0);
  vec3 diffuse = light.diffuse * diffuse_value * ctx.frag_diffuse;

  // Specular
  vec3 reflect_dir = reflect(-light_dir, ctx.frag_normal);
  vec3 specular = (diffuse_value == 0.0 || mesh_shininess == 0.0)
    ? vec3(0.0)
    : light.specular
      * pow(max(dot(ctx.view_dir, reflect_dir), 0.0), mesh_shininess)
      * ctx.frag_specular;

  // Ambient
  vec3 ambient = light.ambient * ctx.frag_diffuse * ctx.ssao;

  // Attenuation
  float dis = length(light.position - vso.position);
  float attenuation = 1.0
    / (light.constant + light.linear * dis + light.quadratic * (dis * dis));

  // Cut Off (soft edge between cut_off and outer_cut_off)
  // cut_off/outer_cut_off are in degrees; convert to cosine for comparison
  // with the dot product theta.
  float theta = dot(light_dir, normalize(-light.direction));
  float cos_cut_off = cos(radians(light.cut_off));
  float cos_outer_cut_off = cos(radians(light.outer_cut_off));
  float epsilon = cos_cut_off - cos_outer_cut_off;
  float intensity = clamp((theta - cos_outer_cut_off) / epsilon, 0.0, 1.0);

  return (ambient + (diffuse + specular) * intensity) * attenuation;
}

/**
 * Point light contribution with distance attenuation (no cutoff cone).
 */
vec3 calc_point_light(Context ctx, PointLight light)
{
  // Diffuse
  vec3 light_dir = normalize(light.position - vso.position);
  float diffuse_value = max(dot(ctx.frag_normal, light_dir), 0.0);
  vec3 diffuse = light.diffuse * diffuse_value * ctx.frag_diffuse;

  // Specular
  vec3 reflect_dir = reflect(-light_dir, ctx.frag_normal);
  vec3 specular = (diffuse_value == 0.0 || mesh_shininess == 0.0)
    ? vec3(0.0)
    : light.specular
      * pow(max(dot(ctx.view_dir, reflect_dir), 0.0), mesh_shininess)
      * ctx.frag_specular;

  // Ambient
  vec3 ambient = light.ambient * ctx.frag_diffuse * ctx.ssao;

  // Attenuation
  float dis = length(light.position - vso.position);
  float attenuation = 1.0
    / (light.constant + light.linear * dis + light.quadratic * (dis * dis));

  return (ambient + diffuse + specular) * attenuation;
}

void main()
{
  // alpha test: discard transparent fragments for non-blended passes
  if (mesh_color.a < alpha_discard) discard;

  Context ctx;
  // world-space view direction (from fragment toward camera)
  ctx.view_dir = normalize(vso.camera_pos - vso.position);

  // parallax occlusion mapping: offset UVs based on the height map and view
  // direction; falls back to the base UVs when no height texture is bound
  vec2 uv = use_height_texture ? calc_height_offset(ctx) : vso.uv;

  // diffuse: gamma-decode the texture then tint with mesh_color
  ctx.frag_diffuse = use_diffuse_texture
    ? pow(texture(diffuse_texture, uv).rgb, vec3(diffuse_texture_gamma))
      * mesh_color.rgb
    : mesh_color.rgb;

  // specular: from texture or uniform
  ctx.frag_specular = use_specular_texture
    ? texture(specular_texture, uv).rgb * mesh_specular
    : vec3(mesh_specular);

  // Normal: normal maps store a tangent-space direction encoded as [0,1] RGB.
  // Decode to [-1,1], then transform to world space via vso.tbn (tangent->world
  // matrix). This is done per-fragment using the single interpolated TBN rather
  // than per-vertex in the VS, so the basis is consistent across the triangle.
  // When no normal map is bound, use the interpolated world-space normal.
  // See mesh3d.vs for why the TBN's handedness (bitangent sign) matters.
  vec3 normal_sample = texture(normal_texture, uv).rgb * 2.0 - 1.0;
  normal_sample.xy *= normal_scale;
  ctx.frag_normal = use_normal_texture ? normalize(vso.tbn * normal_sample)
                                       : normalize(vso.normal);

  // SSAO: sample the blurred occlusion factor (1.0 = unoccluded). The SSAO
  // texture is screen-space, so index it by gl_FragCoord, not mesh UVs.
  ctx.ssao
    = use_ssao ? texture(ssao_texture, gl_FragCoord.xy / viewport).r : 1.0;

  // accumulate light contributions from all active lights
  vec3 result = vec3(0.0);
  for (int i = 0; i < sun_lights_size; ++i)
    result += calc_sun_light(ctx, sun_lights[i]);
  for (int i = 0; i < spot_lights_size; ++i)
    result += calc_spot_light(ctx, spot_lights[i]);
  for (int i = 0; i < point_lights_size; ++i)
    result += calc_point_light(ctx, point_lights[i]);

#ifdef AXGL_DEBUG
  // Debug: draw borders at the edges of each cascade's ortho frustum box
  // (light clip space [-1,1]^3) so nested frustum boundaries are visible as
  // concentric outlines where they intersect scene geometry.
  if (csm_debug_borders && enable_sun_shadow)
  {
    for (int i = 0; i < SUN_SHADOW_CASCADE_COUNT; ++i)
    {
      vec3 proj
        = vso.light_space_position[i].xyz / vso.light_space_position[i].w;
      float min_edge
        = min(min(1.0 - abs(proj.x), 1.0 - abs(proj.y)), 1.0 - abs(proj.z));
      if (min_edge > 0.2) continue;

      float border = 1.0 - smoothstep(0.0, 0.02, abs(min_edge));
      result = mix(result, vec3(0.2, 0.4, 1.0), border * 0.8);
    }
  }
#endif

  // Weighted blended order-independent transparency (OIT):
  // weight by alpha and depth so that transparent surfaces blend correctly
  // without sorted draw order. Opaque passes use weight = 1.0.
#ifndef OPAQUE_PASS
  float weight = transparent ? clamp(
                                 pow(min(1.0, mesh_color.a * 10.0) + 0.01, 3.0)
                                   * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0),
                                 1e-2, 3e3
                               )
                             : 1.0f;
  // pre-multiply color by alpha so the composite recovers the weighted average
  frag_color = vec4(result * mesh_color.a, mesh_color.a) * weight;
  reveal = mesh_color.a;
#else
  // opaque variant: no MRT reveal output, no OIT weighting. straight alpha
  // pass-through (mesh_color.a is 1.0 for typical opaque materials).
  frag_color = vec4(result, mesh_color.a);
#endif
}
