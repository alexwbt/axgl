#pragma once

#include "cube.hpp"
#include <demo_playground/res.hpp>

class BoxEntity final : public CubeEntity
{
public:
  static constexpr std::string_view kTypeId = "entity::box";

protected:
  [[nodiscard]] axgl::ptr_t<axgl::Material> create_material(const axgl::Axgl& axgl) const override
  {
    // textures
    const auto diffuse = axgl.renderer_service()->create_texture();
    const auto specular = axgl.renderer_service()->create_texture();
    diffuse->load_texture(demo_playground_res::get("material/box/diffuse.png"));
    specular->load_texture(demo_playground_res::get("material/box/specular.png"));
    // material
    const auto material = axgl.renderer_service()->create_material("3d");
    material->add_texture(axgl::Material::TextureType::kDiffuse, diffuse);
    material->add_texture(axgl::Material::TextureType::kSpecular, specular);
    return material;
  }
};
