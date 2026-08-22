#pragma once

#include <array>
#include <functional>

#include <axgl/common.hpp>
#include <axgl/interface/light.hpp>

#include <axgl/impl/opengl/cascaded_shadow_map.hpp>
#include <axgl/impl/opengl/texture.hpp>

namespace axgl::impl::opengl::renderer
{

struct LightContext
{
  glm::mat4 light_pv{0.0f};
  // per-cascade light PVs + split distances; populated for sun lights only.
  std::array<CascadedShadowMap::Cascade, CascadedShadowMap::kCascadeCount>
    cascades{};
  const axgl::Light* light = nullptr;
  // must be nullptr-initialized: when shadows are off, gather never sets this
  // and the material reads `shadow_map != nullptr` to detect the shadow light.
  const ::opengl::Texture* shadow_map = nullptr;
};

struct RenderContext
{
  glm::vec2 viewport{0.0f};
  glm::vec3 viewpoint{0.0f};
  glm::mat4 view_matrix{0.0f};
  glm::mat4 projection_matrix{0.0f};
  glm::mat4 projection_view_matrix{0.0f};
  // inverse of projection_view_matrix; used to unproject NDC frustum corners
  // into world space for the cascade frustum fit.
  glm::mat4 inverse_projection_view_matrix{0.0f};
  // real camera near/far (the PV's clip range). the cascade split ratios use
  // these to interpolate the unprojected corners correctly.
  float camera_near = 0.1f;
  float camera_far = 1000.0f;
  std::vector<LightContext> lights{};
  bool csm_debug_borders = false;
  std::int64_t entity_count = 0;
  std::int64_t component_count = 0;
};

struct PipelineContext
{
  std::vector<std::function<void(const LightContext&)>> shadow_pass;
  std::vector<std::function<void(const RenderContext&)>> opaque_pass;
  std::vector<std::function<void(const RenderContext&)>> blend_pass;
};

} // namespace axgl::impl::opengl::renderer
