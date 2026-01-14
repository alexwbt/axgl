#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/components/camera.hpp>
#include <axgl/impl/entity_base.hpp>

#include "debug_cursor.hpp"

class CameraEntity : public axgl::impl::EntityBase
{
public:
  static constexpr std::string_view kTypeId = "entity::camera";

  void on_create(const axgl::Realm::Context& context) override
  {
    const auto& axgl = context.axgl;
    const auto& entity_service = axgl.entity_service();

    auto camera_comp = entity_service->create_component_t<axgl::impl::component::Camera>();
    components().add(std::move(camera_comp));

    // debug cursor
    auto debug_cursor_entity = entity_service->create_entity_t<DebugCursorEntity>();
    context.realm.entities().add(std::move(debug_cursor_entity));
  }
};
