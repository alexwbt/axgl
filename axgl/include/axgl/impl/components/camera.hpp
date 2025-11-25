#pragma once

#include <string_view>

#include <glm/glm.hpp>

#include <axgl/interface/camera.hpp>

#include <axgl/impl/component_base.hpp>

namespace axgl::impl::component
{

class Camera : public ComponentBase
{
public:
  static constexpr std::string_view kType = "component::camera";

  axgl::Camera camera;

  void update() override
  {
    const auto parent = get_entity();
    const auto context = parent->get_context();
    const auto renderer = context->realm->get_renderer();

    if (const auto viewport = glm::vec2(renderer->viewport());
      viewport.x != camera.viewport.x || viewport.y != camera.viewport.y)
    {
      camera.viewport.x = viewport.x;
      camera.viewport.y = viewport.y;
      camera.update_projection_view_matrix();
    }

    if (camera.position != parent->transform()->position)
    {
      camera.position = parent->transform()->position;
      camera.update_projection_view_matrix();
    }

    context->camera = &camera;
  }
};

} // namespace axgl::impl::component
