#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/components/mesh.hpp>
#include <axgl/interface/texture.hpp>

#include <axgl/util/mesh.hpp>

#include "cube.hpp"
#include <demo_playground/res.hpp>

class ConcreteBlockEntity final : public Cube
{
public:
  static constexpr std::string_view kTypeId = "entity::concrete-block";

private:
  static axgl::ptr_t<axgl::component::Mesh> create_mesh(const axgl::Axgl& axgl)
  {
    const auto& entity_service = axgl.entity_service();
    const auto& renderer_service = axgl.renderer_service();
    // textures
    const auto diffuse = axgl.renderer_service()->create_texture();
    const auto specular = axgl.renderer_service()->create_texture();
    const auto normal = axgl.renderer_service()->create_texture();
    const auto height = axgl.renderer_service()->create_texture();
    diffuse->load_texture(demo_playground_res::get("material/concrete/diffuse.png"));
    specular->load_texture(demo_playground_res::get("material/concrete/specular.png"));
    normal->load_texture(demo_playground_res::get("material/concrete/normal.png"));
    height->load_texture(demo_playground_res::get("material/concrete/height.png"));
    // material
    const auto material = renderer_service->create_material("3d");
    material->add_texture(axgl::Material::TextureType::kDiffuse, diffuse);
    material->add_texture(axgl::Material::TextureType::kSpecular, specular);
    material->add_texture(axgl::Material::TextureType::kNormal, normal);
    material->add_texture(axgl::Material::TextureType::kHeight, height);
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
};
