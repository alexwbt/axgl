#pragma once

#include <functional>

#include <axgl/common.hpp>
#include <axgl/interface/light.hpp>

#include <axgl/impl/opengl/texture.hpp>

namespace axgl::impl::opengl::renderer
{

struct LightContext
{
  glm::mat4 light_pv{0.0f};
  const axgl::Light* light = nullptr;
  const ::opengl::Texture* shadow_map;
};

struct RenderContext
{
  glm::vec2 viewport{0.0f};
  glm::vec3 viewpoint{0.0f};
  glm::mat4 view_matrix{0.0f};
  glm::mat4 projection_matrix{0.0f};
  glm::mat4 projection_view_matrix{0.0f};
  std::vector<LightContext> lights;
};

struct PipelineContext
{
  std::vector<std::function<void(const LightContext&)>> shadow_pass;
  std::vector<std::function<void(const RenderContext&)>> opaque_pass;
  std::vector<std::function<void(const RenderContext&)>> blend_pass;
};

} // namespace axgl::impl::opengl::renderer
