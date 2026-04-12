#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/components/camera.hpp>
#include <axgl/impl/entity_base.hpp>

#include "debug_axes_indicator.hpp"

class CameraEntity : public axgl::impl::EntityBase
{
public:
  static constexpr std::string_view kTypeId = "entity::camera";

private:
  axgl::ptr_t<axgl::impl::component::Camera> camera_;
  axgl::ptr_t<axgl::impl::component::Light> flashlight_;
  axgl::ptr_t<DebugAxesIndicatorEntity> debug_cursor_;

  axgl::ptr_t<axgl::Input> flashlight_input_;

public:
  void on_create() override
  {
    EntityBase::on_create();

    const auto& entity_service = axgl_->entity_service();
    const auto& input_manager = axgl_->get_service_t<InputManager>();

    // camera
    camera_ = entity_service->create_component_t<axgl::impl::component::Camera>();
    add_component(camera_);

    // flashlight
    flashlight_ = entity_service->create_component_t<axgl::impl::component::Light>();
    flashlight_->light = axgl::Light::spotlight();
    flashlight_->set_disabled(true);
    add_component(flashlight_);

    // debug cursor
    debug_cursor_ = entity_service->create_entity_t<DebugAxesIndicatorEntity>();
    debug_cursor_->set_scale(glm::vec3(0.05f));
    add_child(debug_cursor_);

    // inputs
    flashlight_input_ = input_manager->flashlight();
  }

  void update() override
  {
    EntityBase::update();

    const auto front = camera_->camera.front();

    // update debug cursor
    if (!debug_cursor_->is_disabled()) debug_cursor_->set_position(front);

    // update flashlight
    if (!flashlight_->is_disabled())
    {
      auto& flashlight_dir = flashlight_->light.direction;
      flashlight_dir += (front - flashlight_dir) * 0.3f;
    }
    // toggle flashlight
    if (flashlight_input_ && flashlight_input_->tick == 1) flashlight_->set_disabled(!flashlight_->is_disabled());
  }
};
