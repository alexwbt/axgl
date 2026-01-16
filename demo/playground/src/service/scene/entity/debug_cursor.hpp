#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/components/mesh.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/entity_base.hpp>

class DebugCursorEntity : public axgl::impl::EntityBase
{
public:
  static constexpr std::string_view kTypeId = "entity::debug-cursor-entity";

private:
  static axgl::ptr_t<axgl::Component> create_axis_mesh(const axgl::Axgl& axgl, const glm::vec3& axis)
  {
    const auto& entity_service = axgl.entity_service();
    const auto& renderer_service = axgl.renderer_service();

    auto material = renderer_service->create_material("color");
    material->set_draw_mode(axgl::Material::DrawMode::kLines);
    material->set_color(glm::vec4(axis, 1.0f));
    material->set_enable_depth_test(false);

    const auto mesh = entity_service->create_component_t<axgl::component::Mesh>();
    mesh->set_vertices(std::vector{{0.0f, 0.0f, 0.0f}, axis});
    mesh->set_material(std::move(material));

    return mesh;
  }

public:
  void on_create(const axgl::Realm::Context& context) override
  {
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
