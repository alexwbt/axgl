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
  static auto create_axis_mesh(const axgl::Axgl& axgl, const glm::vec3& axis)
  {
    const auto& entity_service = axgl.entity_service();
    const auto& renderer_service = axgl.renderer_service();

    const auto material = renderer_service->create_material("color");
    material->set_draw_mode(axgl::Material::DrawMode::kLines);
    material->set_color(glm::vec4(axis, 1.0f));

    const auto mesh = entity_service->create_component_t<axgl::component::Mesh>();
    mesh->set_vertices(std::vector{{0.0f, 0.0f, 0.0f}, axis});
    mesh->set_material(material);

    return mesh;
  }

public:
  void on_create(const axgl::Realm::Context& context) override
  {
    components().add(create_axis_mesh(context.axgl, {1.0f, 0.0f, 0.0f}));
    components().add(create_axis_mesh(context.axgl, {0.0f, 1.0f, 0.0f}));
    components().add(create_axis_mesh(context.axgl, {0.0f, 0.0f, 1.0f}));
  }
};
