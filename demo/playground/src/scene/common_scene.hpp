#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/camera/keyboard_3d_free_fly_camera_mode.hpp>
#include <axgl/impl/components/camera.hpp>

#include "../entity/debug_cursor.hpp"

inline axgl::ptr_t<axgl::Realm> common_scene(const axgl::Axgl& axgl)
{
  const auto& realm_service = axgl.realm_service();
  const auto& entity_service = axgl.entity_service();
  const auto& camera_service = axgl.camera_service();

  const auto realm = realm_service->create_realm();

  // camera entity
  const auto camera_entity = entity_service->create_entity();
  const auto camera_comp = entity_service->create_component_t<axgl::impl::component::Camera>();
  camera_entity->components().add(camera_comp);
  camera_entity->transform().position.y = 2.0f;
  realm->entities().add(camera_entity);
  // camera input
  camera_service->set_camera_mode(axgl::create_ptr<axgl::impl::camera::Keyboard3DFreeFlyCameraMode>());
  camera_service->set_camera(camera_entity);

  // debug cursor
  const auto debug_cursor_entity = entity_service->create_entity_t<DebugCursorEntity>();
  realm->entities().add(debug_cursor_entity);

  return realm;
}
