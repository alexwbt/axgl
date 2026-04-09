#pragma once

#include "cube.hpp"

class BoxEntity final : public CubeEntity
{
public:
  static constexpr std::string_view kTypeId = "entity::box";

protected:
  [[nodiscard]] axgl::ptr_t<axgl::Material> create_material() const override
  {
    const auto& renderer_service = axgl_->renderer_service();
    const auto& resource_service = axgl_->resource_service();
    // textures
    const auto diffuse = renderer_service->create_texture();
    const auto specular = renderer_service->create_texture();
    diffuse->load_texture(resource_service->get_resource("material/box/diffuse.png"));
    specular->load_texture(resource_service->get_resource("material/box/specular.png"));
    // material
    const auto material = renderer_service->create_material("3d");
    material->add_texture(axgl::Material::TextureType::kDiffuse, diffuse);
    material->add_texture(axgl::Material::TextureType::kSpecular, specular);
    return material;
  }
};
