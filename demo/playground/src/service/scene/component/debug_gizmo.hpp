#pragma once

#include <axgl/interface/components/mesh.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/component_base.hpp>

class DebugGizmoComponent : public axgl::impl::ComponentBase
{
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
  static constexpr std::string_view kTypeId = "component::debug-gizmo";

private:
  axgl::ptr_t<axgl::Entity> gizmo_entity_;

public:
  void on_entity_create(const axgl::Entity::Context& context) override
  {
    context.realm.entities().add(gizmo_entity_);

    gizmo_entity_->components().add_all(
      std::array{
        create_axis_mesh(context.axgl, {1.0f, 0.0f, 0.0f}),
        create_axis_mesh(context.axgl, {0.0f, 1.0f, 0.0f}),
        create_axis_mesh(context.axgl, {0.0f, 0.0f, 1.0f}),
      });
    gizmo_entity_->transform().scale = glm::vec3(0.05f);
    gizmo_entity_->update_model_matrix();
  }
};
