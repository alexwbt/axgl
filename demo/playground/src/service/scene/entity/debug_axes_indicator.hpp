#pragma once

#include <axgl/common.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/entity_base.hpp>

class DebugAxesIndicatorEntity : public axgl::impl::EntityBase
{
  static axgl::ptr_t<axgl::Component> create_axis_mesh(const axgl::Axgl* axgl, const glm::vec3& axis)
  {
    const auto& entity_service = axgl->entity_service();
    const auto& renderer_service = axgl->renderer_service();
    auto material = renderer_service->create_material("color");
    material->set_draw_mode(axgl::Material::DrawMode::kLines);
    material->set_color(glm::vec4(axis, 1.0f));
    material->set_enable_depth_test(false);
    material->set_enable_shadow(false);
    const auto mesh = entity_service->create_component_t<axgl::component::Mesh>();
    mesh->set_vertices(std::vector{{0.0f, 0.0f, 0.0f}, axis});
    mesh->set_material(std::move(material));
    return mesh;
  }

  axgl::ptr_t<axgl::Input> debug_input_;

public:
  static constexpr std::string_view kTypeId = "entity::debug-gizmo";

  void on_create() override
  {
    EntityBase::on_create();

    debug_input_ = axgl_->get_service_t<InputManager>()->debug();

    components().add_all(
      std::array{
        create_axis_mesh(axgl_, {1.0f, 0.0f, 0.0f}),
        create_axis_mesh(axgl_, {0.0f, 1.0f, 0.0f}),
        create_axis_mesh(axgl_, {0.0f, 0.0f, 1.0f}),
      });
    set_disabled(true);
  }

  void update() override
  {
    EntityBase::update();

    if (debug_input_->tick == 1) set_disabled(!is_disabled());
  }
};
