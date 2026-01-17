#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/components/camera.hpp>
#include <axgl/impl/entity_base.hpp>

class CameraEntity : public axgl::impl::EntityBase
{
public:
  static constexpr std::string_view kTypeId = "entity::camera";

private:
  axgl::ptr_t<axgl::impl::component::Camera> camera_;
  axgl::ptr_t<axgl::impl::component::Light> flashlight_;

  axgl::ptr_t<axgl::Input> flashlight_input_;

public:
  void on_create(const axgl::Realm::Context& context) override
  {
    EntityBase::on_create(context);

    const auto& axgl = context.axgl;
    const auto& entity_service = axgl.entity_service();

    // camera
    camera_ = entity_service->create_component_t<axgl::impl::component::Camera>();
    components().add(camera_);

    // flashlight
    flashlight_ = entity_service->create_component_t<axgl::impl::component::Light>();
    flashlight_->light = axgl::Light::spotlight();
    flashlight_->set_disabled(true);
    components().add(flashlight_);
  }

  void update(const axgl::Realm::Context& context) override
  {
    EntityBase::update(context);

    const auto target = camera_->camera.front();
    auto& current = flashlight_->light.direction;
    current += (target - current) * 0.3f;

    if (flashlight_input_ && flashlight_input_->tick == 1) flashlight_->set_disabled(!flashlight_->is_disabled());
  }

  void set_flashlight_input(axgl::ptr_t<axgl::Input> flashlight_input)
  {
    flashlight_input_ = std::move(flashlight_input);
  }
};
