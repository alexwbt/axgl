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
  axgl::ptr_t<axgl::Input> debug_input_;

  axgl::ptr_t<FloorEntity> floor_entity_;
  axgl::ptr_t<CameraEntity> camera_entity_;
  axgl::ptr_t<DebugCursorEntity> debug_cursor_entity_;

public:
  CommonScene(const axgl::Axgl& axgl, const InputManager& input_manager)
  {
    const auto& entity_service = axgl.entity_service();
    const auto& camera_service = axgl.camera_service();

    // floor entity
    floor_entity_ = entity_service->create_entity_t<FloorEntity>();

    // camera entity
    camera_entity_ = entity_service->create_entity_t<CameraEntity>();
    camera_entity_->set_flashlight_input(input_manager.flashlight());
    camera_entity_->transform().position.y = 2.0f;
    camera_service->set_camera_mode(axgl::create_ptr<axgl::impl::camera::Keyboard3DFreeFlyCameraMode>());
    camera_service->set_camera_entity(camera_entity_);

    // debug cursor
    debug_input_ = input_manager.debug();
    debug_cursor_entity_ = entity_service->create_entity_t<DebugCursorEntity>();
    debug_cursor_entity_->set_disabled(true);

    entities_.add(floor_entity_);
    entities_.add(camera_entity_);
    entities_.add(debug_cursor_entity_);
  }

  void update(const axgl::Service::Context& context) override
  {
    axgl::impl::Realm::update(context);

    if (debug_input_->tick == 1) debug_cursor_entity_->set_disabled(!debug_cursor_entity_->is_disabled());
  }
};
