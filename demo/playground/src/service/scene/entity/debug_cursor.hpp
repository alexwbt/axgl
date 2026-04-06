#pragma once

#include <axgl/common.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/entity_base.hpp>

class DebugCursorEntity : public axgl::impl::EntityBase
{
public:
  static constexpr std::string_view kTypeId = "entity::debug-cursor-entity";

  void on_create(const axgl::Realm::Context& context) override
  {
    EntityBase::on_create(context);

    components().add_all(
      std::array{
        create_axis_mesh(context.axgl, {1.0f, 0.0f, 0.0f}),
        create_axis_mesh(context.axgl, {0.0f, 1.0f, 0.0f}),
        create_axis_mesh(context.axgl, {0.0f, 0.0f, 1.0f}),
      });
    transform().scale = glm::vec3(0.05f);
    update_model_matrix();
  }

  void update(const axgl::Realm::Context& context) override
  {
    EntityBase::update(context);

    const auto& camera_service = context.axgl.camera_service();
    const auto* camera = camera_service->get_camera();
    if (const auto& camera_entity = camera_service->get_camera_entity(); camera_entity && camera)
    {
      transform().position = camera_entity->transform().position + camera->front();
      update_model_matrix();
    }
  }
};
