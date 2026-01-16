#pragma once

#include <functional>
#include <random>

#include <axgl/axgl.hpp>
#include <axgl/util/mesh.hpp>

#include "box.hpp"

inline axgl::ptr_t<axgl::Texture> create_texture(const axgl::Axgl& axgl, const std::string& res)
{
  // texture
  const auto texture = axgl.renderer_service()->create_texture();
  texture->load_texture(axgl.resource_service()->get_resource(res));
  return texture;
}

inline axgl::ptr_t<axgl::component::Mesh> create_mesh(
  const axgl::Axgl& axgl,
  const std::string& material_type,
  const std::function<void(axgl::component::Mesh&)>& init_mesh,
  const axgl::ptr_t<axgl::Texture>& diffuse_texture = nullptr,
  const axgl::ptr_t<axgl::Texture>& specular_texture = nullptr,
  const float alpha_discard = 0.0f,
  const bool no_cull = false)
{
  const auto entity_service = axgl.entity_service();
  const auto renderer_service = axgl.renderer_service();
  // material
  const auto material = renderer_service->create_material(material_type);
  if (diffuse_texture) material->add_texture(axgl::Material::TextureType::kDiffuse, diffuse_texture);
  if (specular_texture) material->add_texture(axgl::Material::TextureType::kSpecular, specular_texture);
  if (no_cull) material->set_cull_mode(axgl::Material::CullMode::kNone);
  material->set_alpha_discard(alpha_discard);
  // mesh
  const auto mesh = entity_service->create_component_t<axgl::component::Mesh>();
  init_mesh(*mesh);
  mesh->set_material(material);
  return mesh;
}

inline axgl::ptr_t<axgl::Entity> create_grass(const axgl::Axgl& axgl)
{
  static const auto texture = create_texture(axgl, "grass.png");
  static const auto mesh = create_mesh(axgl, "2d", axgl::util::init_quad, texture, nullptr, 0.5f, true);

  // grass entity
  const auto grass = axgl.entity_service()->create_entity();
  grass->components().add(mesh);
  return grass;
}

inline axgl::ptr_t<axgl::Entity> create_box(const axgl::Axgl& axgl)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution dis(-0.05, 0.05);

  static const auto diffuse = create_texture(axgl, "container/diffuse.png");
  static const auto specular = create_texture(axgl, "container/specular.png");
  static const auto mesh = create_mesh(axgl, "blinn-phong", axgl::util::init_cube, diffuse, specular);

  // box entity
  const auto box = axgl.entity_service()->create_entity_t<Box>();
  box->set_rotation_speed({dis(gen), dis(gen), dis(gen)});
  box->set_orbit_speed(static_cast<float>(dis(gen)));
  box->components().add(mesh);
  return box;
}

inline std::vector<axgl::ptr_t<axgl::Entity>> generate_entities(
  const axgl::Axgl& axgl,
  const axgl::ptr_t<axgl::Realm>& realm,
  const std::function<axgl::ptr_t<axgl::Entity>(const axgl::Axgl&)>& create_entity,
  const int count,
  const float area,
  const float y,
  const bool rotate3d)
{
  std::vector<axgl::ptr_t<axgl::Entity>> entities;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution pos_dis(-area, area);
  std::uniform_real_distribution rot_dis(0.0f, 3.1415f);
  for (int i = 0; i < count; i++)
  {
    auto entity = create_entity(axgl);
    auto& transform = entity->transform();
    transform.position.x = pos_dis(gen);
    transform.position.y = y;
    transform.position.z = pos_dis(gen);
    transform.rotation.y = rot_dis(gen);
    if (rotate3d) transform.rotation.x = rot_dis(gen);
    entity->update_model_matrix();
    realm->entities().add(entity);
    entities.emplace_back(std::move(entity));
  }
  return entities;
}
