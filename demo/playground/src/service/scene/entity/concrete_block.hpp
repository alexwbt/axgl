#pragma once

#include "cube.hpp"

class ConcreteBlockEntity final : public CubeEntity
{
public:
  static constexpr std::string_view kTypeId = "entity::concrete-block";

protected:
  [[nodiscard]] axgl::ptr_t<axgl::Material> create_material() const override
  {
    const auto& renderer_service = axgl_->renderer_service();
    const auto& resource_service = axgl_->resource_service();
    // textures
    const auto diffuse = renderer_service->create_texture();
    const auto specular = renderer_service->create_texture();
    const auto normal = renderer_service->create_texture();
    const auto height = renderer_service->create_texture();
    diffuse->load_texture(resource_service->get_resource("material/concrete/diffuse.png"));
    specular->load_texture(resource_service->get_resource("material/concrete/specular.png"));
    normal->load_texture(resource_service->get_resource("material/concrete/normal.png"));
    height->load_texture(resource_service->get_resource("material/concrete/height.png"));
    // material
    const auto material = renderer_service->create_material("3d");
    material->add_texture(axgl::Material::TextureType::kDiffuse, diffuse);
    material->add_texture(axgl::Material::TextureType::kSpecular, specular);
    material->add_texture(axgl::Material::TextureType::kNormal, normal);
    material->add_texture(axgl::Material::TextureType::kHeight, height);
    return material;
  }
};
