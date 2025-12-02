#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/camera.hpp>

#include <axgl/impl/component_base.hpp>

namespace axgl::impl::component
{

class Camera : public axgl::impl::ComponentBase
{
public:
  static constexpr std::string_view kTypeId = "component::camera";

  axgl::Camera camera;

  void update(const axgl::Entity::Context& context) override
  {
    if (const auto viewport = glm::vec2(context.renderer->viewport());
        viewport.x != camera.viewport.x || viewport.y != camera.viewport.y)
    {
      camera.viewport.x = viewport.x;
      camera.viewport.y = viewport.y;
      camera.update_projection_view_matrix();
    }

    if (camera.position != context.entity.transform().position)
    {
      camera.position = context.entity.transform().position;
      camera.update_projection_view_matrix();
    }
  }
};

} // namespace axgl::impl::component
