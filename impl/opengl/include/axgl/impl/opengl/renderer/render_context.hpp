#pragma once

#include <array>
#include <functional>

#include <axgl/common.hpp>
#include <axgl/interface/light.hpp>

#include <axgl/impl/opengl/renderer/constants.hpp>
#include <axgl/impl/opengl/renderer/sun_shadow_cascade.hpp>
#include <axgl/impl/opengl/texture.hpp>

namespace axgl::impl::opengl::renderer
{

struct SunLightContext
{
  const axgl::Light* light = nullptr;
  std::array<SunShadowCascade, kSunShadowCascadeCount> cascades{};
};

struct SpotLightContext
{
  const axgl::Light* light = nullptr;
  std::int32_t shadow_index = -1;
  glm::mat4 light_pv{0.0f};
};

struct PointLightContext
{
  const axgl::Light* light = nullptr;
};

struct RenderContext
{
  glm::vec2 viewport{0.0f};
  glm::vec3 viewpoint{0.0f};
  glm::mat4 view_matrix{0.0f};
  glm::mat4 projection_matrix{0.0f};
  glm::mat4 projection_view_matrix{0.0f};
  glm::mat4 inverse_projection_view_matrix{0.0f};
  float camera_near = 0.1f;
  float camera_far = 1000.0f;
  std::vector<SunLightContext> sun_lights{};
  std::vector<SpotLightContext> spot_lights{};
  std::vector<PointLightContext> point_lights{};
#ifdef AXGL_DEBUG
  bool csm_debug_borders = false;
#endif
  const ::opengl::Texture* sun_shadow_maps = nullptr;
  const ::opengl::Texture* spot_shadow_maps = nullptr;
  const ::opengl::Texture* ssao_texture = nullptr;
  std::int64_t entity_count = 0;
  std::int64_t component_count = 0;
};

struct ShadowPassContext
{
  glm::mat4 projection_view_matrix{0.0f};
};

struct PipelineContext
{
  std::vector<std::function<void(const RenderContext&)>> opaque_pass;
  std::vector<std::function<void(const RenderContext&)>> blend_pass;
  std::vector<std::function<void(const ShadowPassContext&)>> shadow_pass;
  // SSAO geometry pass: renders view-space position + normal to the SSAO
  // g-buffer. Only opaque, shadow-eligible meshes contribute (same set as the
  // shadow pass).
  std::vector<std::function<void(const RenderContext&)>> geometry_pass;
};

} // namespace axgl::impl::opengl::renderer
