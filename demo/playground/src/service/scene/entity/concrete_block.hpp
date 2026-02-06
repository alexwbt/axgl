#pragma once

#include "cube.hpp"
#include <demo_playground/res.hpp>

class ConcreteBlockEntity final : public CubeEntity
{
public:
  static constexpr std::string_view kTypeId = "entity::concrete-block";

protected:
  [[nodiscard]] axgl::ptr_t<axgl::Material> create_material(const axgl::Axgl& axgl) const override
  {
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
    const auto material = axgl.renderer_service()->create_material("3d");
    material->add_texture(axgl::Material::TextureType::kDiffuse, diffuse);
    material->add_texture(axgl::Material::TextureType::kSpecular, specular);
    material->add_texture(axgl::Material::TextureType::kNormal, normal);
    material->add_texture(axgl::Material::TextureType::kHeight, height);
    return material;
  }
};
