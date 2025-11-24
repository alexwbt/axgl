#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>

namespace axgl::impl::component
{

class Camera : public ComponentBase
{
public:
  interface::Camera camera;

  void update() override
  {
    ZoneScopedN("Camera Component Update");

    const auto parent = get_parent();
    const auto context = get_context();
    const auto renderer = context->realm->get_renderer();

    const auto viewport = glm::vec2(renderer->viewport());
    if (viewport.x != camera.viewport.x || viewport.y != camera.viewport.y)
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

} // namespace impl::component

namespace axgl
{

template <> inline std::shared_ptr<impl::component::Camera> interface::RealmService::create_component()
{
  return std::make_shared<impl::component::Camera>();
}

}
