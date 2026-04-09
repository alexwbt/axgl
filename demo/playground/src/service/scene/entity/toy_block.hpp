#pragma once

#include "cube.hpp"
#include <demo_playground/res.hpp>

class ToyBlockEntity final : public CubeEntity
{
public:
  static constexpr std::string_view kTypeId = "entity::toy-block";

protected:
  [[nodiscard]] axgl::ptr_t<axgl::Material> create_material() const override
  {
    const auto& renderer_service = axgl_->renderer_service();
    const auto& resource_service = axgl_->resource_service();
    // textures
    const auto diffuse = renderer_service->create_texture();
    const auto normal = renderer_service->create_texture();
    const auto height = renderer_service->create_texture();
    diffuse->load_texture(resource_service->get_resource("material/toy/diffuse.png"));
    normal->load_texture(resource_service->get_resource("material/toy/normal.png"));
    height->load_texture(resource_service->get_resource("material/toy/height.png"));
    // material
    const auto material = renderer_service->create_material("3d");
    material->add_texture(axgl::Material::TextureType::kDiffuse, diffuse);
    material->add_texture(axgl::Material::TextureType::kNormal, normal);
    material->add_texture(axgl::Material::TextureType::kHeight, height);
    return material;
  }
};
