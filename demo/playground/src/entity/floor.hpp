#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/components/mesh.hpp>
#include <axgl/interface/texture.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/entity_base.hpp>
#include <axgl/util/mesh.hpp>

#include <demo_playground/res.hpp>

class FloorEntity : public axgl::impl::EntityBase
{
public:
  static constexpr std::string_view kTypeId = "entity::floor";

private:
  static axgl::ptr_t<axgl::component::Mesh> create_mesh(const axgl::Axgl& axgl)
  {
    const auto& entity_service = axgl.entity_service();
    const auto& renderer_service = axgl.renderer_service();
    // textures
    auto normal = renderer_service->create_texture();
    auto diffuse = renderer_service->create_texture();
    auto specular = renderer_service->create_texture();
    normal->load_texture(demo_playground_res::get("material/wood/normal.png"));
    diffuse->load_texture(demo_playground_res::get("material/wood/diffuse.png"));
    specular->load_texture(demo_playground_res::get("material/wood/specular.png"));
    // material
    const auto material = renderer_service->create_material("phong");
    material->add_texture(axgl::Material::TextureType::kNormal, std::move(normal));
    material->add_texture(axgl::Material::TextureType::kDiffuse, std::move(diffuse));
    material->add_texture(axgl::Material::TextureType::kSpecular, std::move(specular));
    material->set_tiling({10.0f, 10.0f});
    // mesh
    const auto mesh = entity_service->create_component_t<axgl::component::Mesh>();
    axgl::util::init_plain(*mesh);
    mesh->set_material(material);
    return mesh;
  }

public:
  void on_create(const axgl::Realm::Context& context) override
  {
    static const auto mesh = create_mesh(context.axgl);
    components_.add(mesh);
    transform().scale = {30.0f, 1.0f, 30.0f};
    update_model_matrix();
  }
};
