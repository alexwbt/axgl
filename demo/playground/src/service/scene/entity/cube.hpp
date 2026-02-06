#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/entity_base.hpp>

class CubeEntity : public axgl::impl::EntityBase
{
public:
  static constexpr std::string_view kTypeId = "entity::cube";

private:
  bool rotate_ = false;
  glm::vec3 rotation_speed_{0.0f};
  glm::vec4 color_{0.392f, 0.584f, 0.929f, 1.0f};

protected:
  [[nodiscard]] virtual axgl::ptr_t<axgl::Material> create_material(const axgl::Axgl& axgl) const
  {
    const auto material = axgl.renderer_service()->create_material("3d");
    material->set_color(color_);
    return material;
  }

  [[nodiscard]] virtual axgl::ptr_t<axgl::component::Mesh> create_mesh(const axgl::Axgl& axgl) const
  {
    const auto mesh = axgl.entity_service()->create_component_t<axgl::component::Mesh>();
    mesh->set_material(create_material(axgl));
    axgl::util::init_cube(*mesh);
    return mesh;
  }

public:
  void on_create(const axgl::Realm::Context& context) override
  {
    EntityBase::on_create(context);
    components_.add(create_mesh(context.axgl));
  }

  void update(const axgl::Realm::Context& context) override
  {
    EntityBase::update(context);

    if (rotate_)
    {
      auto& transform = this->transform();
      const auto delta_tick = static_cast<float>(context.delta_tick);

      transform.rotation += rotation_speed_ * delta_tick;
      update_model_matrix();
    }
  }

  void set_rotation_speed(glm::vec3 rotation_speed)
  {
    rotation_speed_ = rotation_speed;
    rotate_ = true;
  }

  void set_color(glm::vec4 color) { color_ = color; }
};
