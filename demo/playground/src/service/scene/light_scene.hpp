#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/realm.hpp>

#include "../input_manager.hpp"
#include "common_scene.hpp"

class LightScene : public CommonScene
{
public:
  LightScene(const axgl::Axgl& axgl, const InputManager& input_manager) : CommonScene(axgl, input_manager)
  {
    const auto& entity_service = axgl.entity_service();

    // spotlight
    {
      auto light_entity = entity_service->create_entity();
      auto light_comp = entity_service->create_component_t<axgl::impl::component::Light>();
      light_comp->light = axgl::Light::spotlight({0.2f, -1.0f, 1.2f});
      light_entity->components().add(std::move(light_comp));
      light_entity->transform().position.y = 1.0f;
      light_entity->update_model_matrix();
      entities_.add(std::move(light_entity));
    }

    // point light
    {
      auto light_entity = entity_service->create_entity();
      auto light_comp = entity_service->create_component_t<axgl::impl::component::Light>();
      light_comp->light = axgl::Light::point_light();
      light_entity->components().add(std::move(light_comp));
      light_entity->transform().position = {20.0f, 1.0f, 0.0f};
      light_entity->update_model_matrix();
      entities_.add(std::move(light_entity));
    }
  }
};
