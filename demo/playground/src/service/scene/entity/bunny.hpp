#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/entity_base.hpp>

class BunnyEntity : public axgl::impl::EntityBase
{
  static void load_model(const axgl::Axgl* axgl, BunnyEntity* entity)
  {
    static const auto resources = axgl->model_service()->load_model("bunny.glb");
    for (auto& components = entity->components(); const auto& mesh : resources.meshes)
      components.add(mesh);
  }

public:
  static constexpr std::string_view kTypeId = "entity::bunny";

  void on_create() override
  {
    EntityBase::on_create();

    load_model(axgl_, this);
    // transform().origin = {0.25f, 0.25f, 0.25f};

    children().add(axgl_->entity_service()->create_entity_t<DebugGizmoEntity>());
  }
};
