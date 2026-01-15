#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>

#include "../entity/floor.hpp"
#include "common_scene.hpp"

inline axgl::ptr_t<axgl::Realm> blinn_scene(const axgl::Axgl& axgl)
{
  const auto& entity_service = axgl.entity_service();

  const auto realm = common_scene(axgl);

  // light entity
  // {
  //   auto light_entity = entity_service->create_entity();
  //   auto light_comp = entity_service->create_component_t<axgl::impl::component::Light>();
  //   light_comp->light = axgl::Light(
  //     glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
  //     axgl::Light::Color{
  //       .ambient = glm::vec3(0.01f),
  //       .diffuse = glm::vec3(1.0f),
  //       .specular = glm::vec3(1.0f),
  //     },
  //     axgl::Light::Strength{
  //       .constant = 1.0f,
  //       .linear = 0.09f,
  //       .quadratic = 0.032f,
  //     },
  //     12.5f, 15.0f);
  //   light_entity->components().add(std::move(light_comp));
  //   light_entity->transform().rotation = glm::vec3(0.2f, -1.0f, 1.2f);
  //   realm->entities().add(std::move(light_entity));
  // }

  // floor entity
  {
    auto floor_entity = entity_service->create_entity_t<FloorEntity>();
    realm->entities().add(std::move(floor_entity));
  }

  return realm;
}
