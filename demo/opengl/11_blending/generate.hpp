#pragma once

#include <random>

#include <axgl/axgl.hpp>
#include <axgl/util/mesh.hpp>

inline axgl::ptr_t<axgl::Texture> create_grass_texture(const axgl::Axgl& axgl)
{
  // texture
  const auto texture = axgl.renderer_service()->create_texture();
  texture->load_texture(axgl.resource_service()->get_resource("grass.png"));
  return texture;
}

inline axgl::ptr_t<axgl::component::Mesh> create_mesh(const axgl::Axgl& axgl, const axgl::ptr_t<axgl::Texture>& texture)
{
  const auto entity_service = axgl.entity_service();
  const auto renderer_service = axgl.renderer_service();
  // material
  const auto material = renderer_service->create_material("2d");
  if (texture)
    material->add_texture(axgl::Material::TextureType::kDiffuse, texture);
  material->set_cull_mode(axgl::Material::CullMode::kNone);
  material->set_enable_blend(true);
  // mesh
  const auto mesh = entity_service->create_component_t<axgl::component::Mesh>();
  axgl::util::init_quad(*mesh);
  mesh->set_material(material);
  return mesh;
}

inline axgl::ptr_t<axgl::Entity> create_grass(const axgl::Axgl& axgl)
{
  static const auto texture = create_grass_texture(axgl);
  static const auto mesh = create_mesh(axgl, texture);

  // grass entity
  const auto grass = axgl.entity_service()->create_entity();
  grass->components().add(mesh);
  return grass;
}

inline axgl::ptr_t<axgl::Entity> create_pane(const axgl::Axgl& axgl)
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution color_dis(0.0f, 1.0f);

  // pane mesh with random colors
  const auto mesh = create_mesh(axgl, nullptr);
  mesh->get_material()->set_color(glm::vec4(color_dis(gen), color_dis(gen), color_dis(gen), color_dis(gen)));
  mesh->get_material()->set_enable_blend(true);

  // pane entity
  const auto pane = axgl.entity_service()->create_entity();
  pane->components().add(mesh);
  return pane;
}

inline void generate_entities(
  const axgl::Axgl& axgl,
  const axgl::ptr_t<axgl::Realm>& realm,
  const std::function<axgl::ptr_t<axgl::Entity>(const axgl::Axgl&)>& create_entity,
  const int count,
  const float area,
  const float y,
  const bool rotate3d)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution pos_dis(-area, area);
  std::uniform_real_distribution rot_dis(0.0f, 3.1415f);
  // grass entities
  for (int i = 0; i < count; i++)
  {
    const auto entity = create_entity(axgl);
    auto& transform = entity->transform();
    transform.position.x = pos_dis(gen);
    transform.position.y = y;
    transform.position.z = pos_dis(gen);
    transform.rotation.y = rot_dis(gen);
    if (rotate3d)
      transform.rotation.x = rot_dis(gen);
    entity->update_model_matrix();
    realm->entities().add(entity);
  }
}

inline void generate(const axgl::Axgl& axgl, const axgl::ptr_t<axgl::Realm>& realm)
{
  generate_entities(axgl, realm, create_grass, 2000, 20.0f, 0.0f, false);
  generate_entities(axgl, realm, create_pane, 100, 5.0f, 5.0f, false);
}
