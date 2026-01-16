#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/realm.hpp>

#include "../input_manager.hpp"
#include "common_scene.hpp"
#include "entity/floor.hpp"

class BlinnScene : public CommonScene
{
public:
  BlinnScene(const axgl::Axgl& axgl, const InputManager& input_manager) : CommonScene(axgl, input_manager)
  {
    const auto& entity_service = axgl.entity_service();

    // light entity
    {
      auto light_entity = entity_service->create_entity();
      auto light_comp = entity_service->create_component_t<axgl::impl::component::Light>();
      light_comp->light = axgl::Light::spotlight({0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f});
      light_entity->components().add(std::move(light_comp));
      entities_.add(std::move(light_entity));
    }

    // floor entity
    {
      auto floor_entity = entity_service->create_entity_t<FloorEntity>();
      entities_.add(std::move(floor_entity));
    }
  }
};
