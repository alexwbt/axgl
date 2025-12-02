#pragma once

#include <random>

#include <axgl/axgl.hpp>

inline std::shared_ptr<axgl::Texture> create_grass_texture(const axgl::Axgl& axgl)
{
  // texture
  const auto texture = axgl.renderer_service()->create_texture();
  texture->load_texture(axgl.resource_service()->get_resource("grass.png"));
  return texture;
}

inline std::shared_ptr<axgl::Entity> create_grass(const axgl::Axgl& axgl)
{
  static const auto texture = create_grass_texture(axgl);

  const auto entity_service = axgl.entity_service();
  const auto renderer_service = axgl.renderer_service();

  // grass entity
  const auto grass = entity_service->create_entity();
  {
    // material
    const auto material = renderer_service->create_material("2d");
    material->add_texture(axgl::Material::TextureType::kDiffuse, texture);
    material->set_cull_mode(axgl::Material::CullMode::kNone);
    material->set_enable_blend(true);

    // grass mesh
    const auto mesh = entity_service->create_component_t<axgl::component::Mesh>();
    mesh->set_vertices(
      std::vector<glm::vec2>{
        {0.5f, 0.5f},
        {0.5f, -0.5f},
        {-0.5f, -0.5f},
        {-0.5f, 0.5f},
      });
    mesh->set_uv(
      std::vector<glm::vec2>{
        {1.0f, 1.0f},
        {1.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 1.0f},
      });
    mesh->set_indices(std::vector<uint32_t>{0, 1, 2, 0, 2, 3});
    mesh->set_material(material);
    grass->components().add(mesh);
  }
  return grass;
}

inline void generate_grass(const axgl::ptr_t<axgl::Realm>& realm, const axgl::Axgl& axgl)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution pos_dis(-10.0f, 10.0f);
  std::uniform_real_distribution rot_dis(0.0f, 3.1415f);
  // grass entities
  for (int i = 0; i < 1000; i++)
  {
    const auto grass = create_grass(axgl);
    grass->transform().position.x = pos_dis(gen);
    grass->transform().position.y = -2;
    grass->transform().position.z = pos_dis(gen);
    grass->transform().rotation.y = rot_dis(gen);
    grass->update_model_matrix();
    realm->entities().add(grass);
  }
}
