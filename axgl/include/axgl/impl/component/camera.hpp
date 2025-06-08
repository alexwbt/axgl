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

    void update()
    {
      if (disabled)
        return;

      auto* parent = get_parent();
      auto* context = get_context();
      auto renderer = context->realm->get_renderer();

      camera.position = parent->position;
      camera.viewport = glm::vec2(renderer->viewport());
      camera.update();

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
