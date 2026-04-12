#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/camera/keyboard_3d_free_fly_camera_mode.hpp>
#include <axgl/impl/realm.hpp>

#include "entity/camera.hpp"
#include "entity/floor.hpp"

class CommonScene : public axgl::impl::Realm
{
protected:
  axgl::ptr_t<axgl::EntityService> entity_service_;
  axgl::ptr_t<InputManager> input_manager_;

  axgl::ptr_t<FloorEntity> floor_entity_;
  axgl::ptr_t<CameraEntity> camera_entity_;

public:
  void initialize() override
  {
    const auto& entity_service = axgl_->entity_service();
    const auto& camera_service = axgl_->camera_service();
    entity_service_ = entity_service;
    input_manager_ = axgl_->get_service_t<InputManager>();

    // floor entity
    floor_entity_ = entity_service->create_entity_t<FloorEntity>();

    // camera entity
    camera_entity_ = entity_service->create_entity_t<CameraEntity>();
    camera_entity_->transform().position.y = 2.0f;
    camera_entity_->transform().update_matrix();
    camera_service->set_camera_mode(axgl::create_ptr<axgl::impl::camera::Keyboard3DFreeFlyCameraMode>());
    camera_service->set_camera_entity(camera_entity_);

    entities_.add(floor_entity_);
    entities_.add(camera_entity_);
  }

protected:
  auto add_light(const axgl::Light& light)
  {
    const auto light_entity = entity_service_->create_entity();
    const auto light_comp = entity_service_->create_component_t<axgl::impl::component::Light>();
    light_comp->light = light;
    light_entity->add_component(light_comp);
    light_entity->set_position(light.position);
    entities_.add(light_entity);
    return light_comp;
  }
};
