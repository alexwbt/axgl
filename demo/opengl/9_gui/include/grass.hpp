#pragma once

#include <axgl/axgl.hpp>
#include <axgl/interface/realm.hpp>

inline std::shared_ptr<axgl::interface::Texture> create_grass_texture(const axgl::Axgl* axgl)
{
  // texture
  const auto texture = axgl->renderer_service()->create_texture();
  texture->load_texture(axgl->resource_service()->get_resource("grass/grass.png"));
  return texture;
}

inline std::shared_ptr<axgl::interface::Entity> create_grass(const axgl::Axgl* axgl)
{
  static const auto texture = create_grass_texture(axgl);

  const auto realm_service = axgl->realm_service();
  const auto renderer_service = axgl->renderer_service();

  // grass entity
  const auto grass = realm_service->create_entity<axgl::interface::Entity>();
  {
    // material
    const auto material = renderer_service->create_material("2d");
    material->add_texture(axgl::interface::TextureType::kDiffuse, texture);
    material->set_cull_mode(axgl::interface::CullMode::kNone);
    material->set_enable_blend(true);

    // grass mesh
    const auto mesh = realm_service->create_component<axgl::interface::component::Mesh>();
    mesh->set_vertices(std::vector<glm::vec2>{
      { 0.5f, 0.5f },
      { 0.5f, -0.5f },
      { -0.5f, -0.5f },
      { -0.5f, 0.5f },
    });
    mesh->set_uv(std::vector<glm::vec2>{
      { 1.0f, 1.0f },
      { 1.0f, 0.0f },
      { 0.0f, 0.0f },
      { 0.0f, 1.0f },
    });
    mesh->set_indices(std::vector<uint32_t>{ 0, 1, 2, 0, 2, 3 });
    mesh->set_material(material);
    grass->add_component(mesh);
  }
  return grass;
}
