#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/components/mesh.hpp>
#include <axgl/interface/texture.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/entity_base.hpp>
#include <axgl/util/mesh.hpp>

#include <demo_playground/res.hpp>

class BoxEntity final : public axgl::impl::EntityBase
{
public:
  static constexpr std::string_view kTypeId = "entity::box";

private:
  bool rotate_ = false;
  glm::vec3 rotation_speed_{0.0f};

  static axgl::ptr_t<axgl::component::Mesh> create_mesh(const axgl::Axgl& axgl)
  {
    const auto& entity_service = axgl.entity_service();
    const auto& renderer_service = axgl.renderer_service();
    // textures
    const auto diffuse = axgl.renderer_service()->create_texture();
    const auto specular = axgl.renderer_service()->create_texture();
    diffuse->load_texture(demo_playground_res::get("material/box/diffuse.png"));
    specular->load_texture(demo_playground_res::get("material/box/specular.png"));
    // material
    const auto material = renderer_service->create_material("3d");
    material->add_texture(axgl::Material::TextureType::kDiffuse, diffuse);
    material->add_texture(axgl::Material::TextureType::kSpecular, specular);
    // mesh
    const auto mesh = entity_service->create_component_t<axgl::component::Mesh>();
    axgl::util::init_cube(*mesh);
    mesh->set_material(material);
    return mesh;
  }

public:
  void on_create(const axgl::Realm::Context& context) override
  {
    EntityBase::on_create(context);

    static const auto mesh = create_mesh(context.axgl);
    components_.add(mesh);
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
};
