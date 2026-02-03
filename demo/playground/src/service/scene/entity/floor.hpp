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
  float size_ = 500.0f;

public:
  void set_size(float size) { size_ = size; }

private:
  static axgl::ptr_t<axgl::component::Mesh> create_mesh(const axgl::Axgl& axgl, float size)
  {
    const auto& entity_service = axgl.entity_service();
    const auto& renderer_service = axgl.renderer_service();
    // textures
    // auto normal = renderer_service->create_texture();
    // auto diffuse = renderer_service->create_texture();
    // auto specular = renderer_service->create_texture();
    // normal->load_texture(demo_playground_res::get("material/wood/normal.png"));
    // diffuse->load_texture(demo_playground_res::get("material/wood/diffuse.png"));
    // specular->load_texture(demo_playground_res::get("material/wood/specular.png"));
    // material
    auto material = renderer_service->create_material("blinn-phong");
    // material->add_texture(axgl::Material::TextureType::kNormal, std::move(normal));
    // material->add_texture(axgl::Material::TextureType::kDiffuse, std::move(diffuse));
    // material->add_texture(axgl::Material::TextureType::kSpecular, std::move(specular));
    // material->set_tiling(glm::vec2(size) / 3.0f);
    material->set_color({0.392f, 0.584f, 0.929f, 1.0f});
    // mesh
    const auto mesh = entity_service->create_component_t<axgl::component::Mesh>();
    axgl::util::init_plain(*mesh);
    mesh->set_material(std::move(material));
    return mesh;
  }

public:
  void on_create(const axgl::Realm::Context& context) override
  {
    EntityBase::on_create(context);

    static const auto mesh = create_mesh(context.axgl, size_);
    components_.add(mesh);
    transform().scale = {size_, 1.0f, size_};
    update_model_matrix();
  }
};
