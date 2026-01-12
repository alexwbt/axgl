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
  {
    const auto light_entity = entity_service->create_entity();
    const auto light_comp = entity_service->create_component_t<axgl::impl::component::Light>();
    light_comp->light.color.ambient = glm::vec3(0.3f);
    light_comp->light.color.specular = glm::vec3(0.0f);
    light_entity->components().add(light_comp);
    light_entity->transform().rotation = glm::vec3(0.2f, -1.0f, 1.2f);
    realm->entities().add(light_entity);
  }

  // floor entity
  {
    const auto floor_entity = entity_service->create_entity_t<FloorEntity>();
    realm->entities().add(floor_entity);
  }

  return realm;
}
