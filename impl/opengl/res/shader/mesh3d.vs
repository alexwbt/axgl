#version 410 core
/**
 * mesh3d.vs - Vertex shader for 3D mesh rendering (Blinn-Phong + normal/height
 * mapping).
 *
 * Outputs world-space position, normal, and a tangent->world TBN matrix to the
 * fragment shader. Lighting and parallax are computed in world space in the FS;
 * the TBN matrix is only used to transform normal-map samples (tangent->world)
 * and to derive the tangent-space view direction for parallax occlusion
 * mapping.
 *
 * Inputs:
 *   location 0: position       - vertex position (object space)
 *   location 1: normal         - vertex normal (object space)
 *   location 2: tangent        - vertex tangent (object space)
 *   location 3: bitangent     - vertex bitangent (object space, used for
 * handedness) location 4: uv            - texture coordinates location 5: model
 * - per-instance model matrix (instanced rendering)
 *
 * Uniforms:
 *   camera_pos          - world-space camera position
 *   projection_view     - combined projection * view matrix
 *   cascade_count       - number of shadow cascades
 *   cascade_light_pv[N] - per-cascade light projection * view matrices
 *   uv_scale / uv_offset- texture coordinate transform
 *   use_normal_texture  - normal map present (enables TBN construction)
 *   use_height_texture  - height map present (enables TBN construction for POM)
 *   enable_shadow       - shadow mapping enabled
 *
 * Outputs (vso):
 *   camera_pos          - world-space camera position (untransformed)
 *   position            - world-space fragment position
 *   normal              - world-space normal (normalized)
 *   uv                  - scaled/offset texture coordinates
 *   tbn                 - tangent->world matrix (columns: t, b, n); identity if
 *                        no normal/height texture
 *   light_space_position[N] - clip-space position in each cascade's light
 * frustum
 *
 * Note: gl_Position.x is negated to match the engine's handedness convention.
 */

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec2 uv;
layout(location = 5) in mat4 model;

uniform vec3 camera_pos;
uniform mat4 projection_view;
uniform int cascade_count;
uniform mat4 cascade_light_pv[CASCADE_COUNT];
uniform vec2 uv_scale;
uniform vec2 uv_offset;
uniform bool use_normal_texture;
uniform bool use_height_texture;
uniform bool enable_shadow;

out VertexShaderOutput
{
  vec3 camera_pos;
  vec3 position;
  vec3 normal;
  vec2 uv;
  mat3 tbn;
  vec4 light_space_position[CASCADE_COUNT];
}
vso;

void main()
{
  vec4 position_v4 = vec4(position, 1.0);
  gl_Position = projection_view * model * position_v4;
  // negate x to match the engine's handedness convention
  gl_Position.x = -gl_Position.x;

  // inverse-transpose for correct normals under non-uniform scaling
  mat3 normal_matrix = transpose(inverse(mat3(model)));

  vso.camera_pos = camera_pos;
  vso.position = vec3(model * position_v4);
  vso.normal = normalize(normal_matrix * normal);
  vso.uv = (uv + uv_offset) * uv_scale;

  if (enable_shadow)
  {
    // output the world-space position transformed by each cascade's light PV
    // so the FS can pick the matching one without re-transforming.
    for (int i = 0; i < cascade_count; ++i)
      vso.light_space_position[i]
        = cascade_light_pv[i] * vec4(vso.position, 1.0);
  }

  // Build a tangent->world TBN matrix when normal or height mapping is active.
  //
  // Handedness: a mesh's UV layout can have inconsistent winding across faces
  // (e.g. init_cube has 6 faces, some right-handed, some left-handed). If we
  // unconditionally set b = cross(n, t) we force every face to right-handed,
  // which flips the V axis of the tangent space on originally left-handed
  // faces. Those faces' normal maps are then mirrored on V, so bumps become
  // dents and the lighting appears to "shift" as the object rotates and
  // different faces come into view.
  //
  // To avoid this, the bitangent's handedness sign (w) is derived from the
  // input bitangent: w = sign(dot(cross(n, t), b_in)). The reconstructed,
  // orthonormal bitangent is then scaled by w so each face's TBN basis matches
  // its UV winding. calculate_tbn() in the C++ mesh component performs the
  // same handedness derivation for procedurally-generated meshes; this shader
  // path handles assimp-loaded meshes that provide their own bitangents.
  if (use_normal_texture || use_height_texture)
  {
    vec3 t = normalize(normal_matrix * tangent);
    vec3 b_in = normalize(normal_matrix * bitangent);
    float w = (dot(cross(vso.normal, t), b_in) < 0.0) ? -1.0 : 1.0;
    vec3 b = w * cross(vso.normal, t);
    vso.tbn = mat3(t, b, vso.normal);
  }
  else vso.tbn = mat3(1.0);
}
