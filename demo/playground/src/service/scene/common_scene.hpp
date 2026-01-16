#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/camera/keyboard_3d_free_fly_camera_mode.hpp>
#include <axgl/impl/realm.hpp>

#include "../input_manager.hpp"
#include "entity/camera.hpp"
#include "entity/debug_cursor.hpp"

class CommonScene : public axgl::impl::Realm
{
  axgl::ptr_t<axgl::Input> debug_input_;
  axgl::ptr_t<DebugCursorEntity> debug_cursor_;

public:
  CommonScene(const axgl::Axgl& axgl, const InputManager& input_manager)
  {
    const auto& entity_service = axgl.entity_service();

    // camera entity
    {
      auto camera_entity = entity_service->create_entity_t<CameraEntity>();
      camera_entity->set_flashlight_input(input_manager.flashlight());
      camera_entity->transform().position.y = 2.0f;
      entities_.add(camera_entity);
      // camera input
      const auto& camera_service = axgl.camera_service();
      camera_service->set_camera_mode(axgl::create_ptr<axgl::impl::camera::Keyboard3DFreeFlyCameraMode>());
      camera_service->set_camera_entity(std::move(camera_entity));
    }

    // debug cursor
    debug_input_ = input_manager.debug();
    debug_cursor_ = entity_service->create_entity_t<DebugCursorEntity>();
    debug_cursor_->set_disabled(true);
    entities_.add(debug_cursor_);
  }

  void update(const axgl::Service::Context& context) override
  {
    axgl::impl::Realm::update(context);

    if (debug_input_->tick == 1) debug_cursor_->set_disabled(!debug_cursor_->is_disabled());
  }
};
