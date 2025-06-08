#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>

NAMESPACE_AXGL_IMPL

namespace component
{
  class Camera : public interface::Component
  {
  public:
    interface::Camera camera;

    void update() override
    {
      if (disabled)
        return;

      auto parent = get_parent();
      auto context = get_context();
      auto renderer = context->realm->get_renderer();

      const auto& viewport = renderer->viewport();
      if (viewport.x != camera.viewport.x || viewport.y != camera.viewport.y)
      {
        camera.viewport.x = viewport.x;
        camera.viewport.y = viewport.y;
        camera.update_projection_view_matrix();
      }

      if (camera.position != parent->position)
      {
        camera.position = parent->position;
        camera.update_projection_view_matrix();
      }

      context->camera = &camera;
    }
  };
}

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::component::Camera> interface::RealmService::create_component()
{
  return std::make_shared<impl::component::Camera>();
}

NAMESPACE_AXGL_END
