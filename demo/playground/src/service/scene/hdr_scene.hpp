#pragma once

#include "common_scene.hpp"

#include <axgl/common/mesh.hpp>

#include "entity/box.hpp"
#include "entity/concrete_block.hpp"
#include "entity/cube.hpp"
#include "entity/toy_block.hpp"

class HdrScene : public CommonScene
{
public:
  static constexpr std::string_view kTypeId = "realm::hdr";

private:
  void add_wall(
    const glm::vec3& position, const glm::vec3& scale, const glm::vec2& tiling
  )
  {
    const auto& renderer_service = axgl_->renderer_service();
    const auto& resource_service = axgl_->resource_service();
    const auto diffuse = renderer_service->create_texture();
    const auto specular = renderer_service->create_texture();
    const auto normal = renderer_service->create_texture();
    diffuse->load_texture(
      resource_service->get_resource("material/concrete/diffuse.png")
    );
    specular->load_texture(
      resource_service->get_resource("material/concrete/specular.png")
    );
    normal->load_texture(
      resource_service->get_resource("material/concrete/normal.png")
    );
    const auto material = renderer_service->create_material("3d");
    material->add_texture(axgl::Material::TextureType::kDiffuse, diffuse);
    material->add_texture(axgl::Material::TextureType::kSpecular, specular);
    material->add_texture(axgl::Material::TextureType::kNormal, normal);
    material->set_tiling(tiling / 2.0f);
    const auto mesh
      = entity_service_->create_component_t<axgl::component::Mesh>();
    axgl::mesh::init_cube(*mesh);
    mesh->set_material(material);
    const auto entity = entity_service_->create_entity();
    entity->add_component(mesh);
    entity->set_position(position);
    entity->set_scale(scale);
    entity->transform().update_matrix();
    entities_.add(entity);
  }

public:
  void on_create() override
  {
    CommonScene::on_create();

    const float half_w = 2.5f;
    const float length = 20.0f;
    const float wall_h = 2.5f;
    const float thickness = 0.1f;

    // ceiling
    add_wall(
      {0.0f, wall_h - thickness * 0.5f, -length * 0.5f},
      {half_w * 2.0f, thickness, length}, {half_w * 2.0f, length}
    );
    // right wall
    add_wall(
      {half_w + thickness * 0.5f, wall_h * 0.5f, -length * 0.5f},
      {thickness, wall_h, length}, {length, wall_h}
    );
    // left wall
    add_wall(
      {-(half_w + thickness * 0.5f), wall_h * 0.5f, -length * 0.5f},
      {thickness, wall_h, length}, {length, wall_h}
    );

    add_entity<CubeEntity>({-1.0f, 0.5f, -4.0f}, 0.5f);
    add_entity<BoxEntity>({1.0f, 0.5f, -7.0f}, 0.5f);
    add_entity<ToyBlockEntity>({0.0f, 0.5f, -10.0f}, 0.5f);
    add_entity<ConcreteBlockEntity>({-1.2f, 0.5f, -13.0f}, 0.5f);
    add_entity<CubeEntity>({1.2f, 0.5f, -15.0f}, 0.5f);

    // bright light at the end of the tunnel (HDR showcase)
    add_light(
      axgl::Light::point_light(
        {0.0f, 1.5f, -length + 1.0f},
        axgl::Light::Strength{1.0f, 0.09f, 0.032f},
        axgl::Light::Color{glm::vec3(5.0f), glm::vec3(80.0f), glm::vec3(80.0f)}
      )
    );

    camera_entity_->transform().position = {0.0f, 1.0f, 3.0f};
    camera_entity_->transform().update_matrix();
  }
};
