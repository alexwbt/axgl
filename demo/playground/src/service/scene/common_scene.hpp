#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/camera/keyboard_3d_free_fly_camera_mode.hpp>
#include <axgl/impl/realm.hpp>

#include "../input_manager.hpp"
#include "entity/camera.hpp"
#include "entity/debug_cursor.hpp"
#include "entity/floor.hpp"

class CommonScene : public axgl::impl::Realm
{
protected:
  axgl::ptr_t<axgl::EntityService> entity_service_;
  axgl::ptr_t<InputManager> inputs_;

  axgl::ptr_t<FloorEntity> floor_entity_;
  axgl::ptr_t<CameraEntity> camera_entity_;
  axgl::ptr_t<DebugCursorEntity> debug_cursor_entity_;

public:
  void initialize(const axgl::Service::Context& context) override
  {
    const auto& axgl = context.axgl;
    const auto& entity_service = axgl.entity_service();
    const auto& camera_service = axgl.camera_service();

    // services
    entity_service_ = entity_service;
    inputs_ = axgl.get_service<InputManager>("input_manager");

    // floor entity
    floor_entity_ = entity_service->create_entity_t<FloorEntity>();

    // camera entity
    camera_entity_ = entity_service->create_entity_t<CameraEntity>();
    camera_entity_->set_flashlight_input(inputs_->flashlight());
    camera_entity_->transform().position.y = 2.0f;
    camera_service->set_camera_mode(axgl::create_ptr<axgl::impl::camera::Keyboard3DFreeFlyCameraMode>());
    camera_service->set_camera_entity(camera_entity_);

    // debug cursor entity
    debug_cursor_entity_ = entity_service->create_entity_t<DebugCursorEntity>();
    debug_cursor_entity_->set_disabled(true);

    entities_.add(floor_entity_);
    entities_.add(camera_entity_);
    entities_.add(debug_cursor_entity_);
  }

  void update(const axgl::Service::Context& context) override
  {
    Realm::update(context);

    if (inputs_->debug()->tick == 1) debug_cursor_entity_->set_disabled(!debug_cursor_entity_->is_disabled());
  }

protected:
  auto add_light(const axgl::Light& light)
  {
    const auto light_entity = entity_service_->create_entity();
    const auto light_comp = entity_service_->create_component_t<axgl::impl::component::Light>();
    light_comp->light = light;
    light_entity->components().add(light_comp);
    light_entity->transform().position = light.position;
    light_entity->update_model_matrix();
    entities_.add(light_entity);
    return light_comp;
  }
};
