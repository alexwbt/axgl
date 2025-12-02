#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>

#include <axgl/impl/entity_container.hpp>

namespace axgl::impl
{

class Realm : public axgl::Realm
{
  axgl::impl::EntityContainer entities_;
  axgl::ptr_t<axgl::Renderer> renderer_;
  axgl::ptr_t<axgl::CameraService> camera_service_;
  std::vector<const axgl::Light*> lights_;

public:
  void tick(const Service::Context& context) override
  {
    AXGL_PROFILE_SCOPE("Realm Tick");

    entities_.tick({context, *this, renderer_, camera_service_->get_camera(), lights_});
  }

  void update(const Service::Context& context) override
  {
    AXGL_PROFILE_SCOPE("Realm Update");

    if (!camera_service_)
      camera_service_ = context.axgl.camera_service();

    entities_.update({context, *this, renderer_, camera_service_->get_camera(), lights_});
  }

  void render(const Service::Context& context) override
  {
    AXGL_PROFILE_SCOPE("Realm Render");

    const auto* camera = camera_service_->get_camera();

    if (!renderer_ || !renderer_->ready() || !camera)
      return;

    renderer_->before_render();
    entities_.render({
      context,
      *this,
      renderer_,
      camera,
      lights_,
    });
    renderer_->after_render();
  }

  void set_renderer(axgl::ptr_t<Renderer> renderer) override { renderer_ = std::move(renderer); }
  [[nodiscard]] axgl::ptr_t<Renderer> get_renderer() const override { return renderer_; }

  [[nodiscard]] EntityContainer& entities() override { return entities_; }
};

} // namespace axgl::impl
