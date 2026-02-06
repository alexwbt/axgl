#pragma once

#include "common_scene.hpp"

#include "entity/box.hpp"
#include "entity/concrete_block.hpp"

class TestingScene : public CommonScene
{
  float x_ = 0.0f;

  glm::vec3 item_position(float height, float margin = 2.0f)
  {
    x_ += margin * 2.0f;
    return {x_ - margin, height, 0.0f};
  }

  void add_entity(const axgl::ptr_t<axgl::Entity>& entity)
  {
    entity->update_model_matrix();
    entities_.add(entity);
  }

public:
  void initialize(const axgl::Service::Context& context) override
  {
    CommonScene::initialize(context);

    // sunlight
    add_light(axgl::Light::sunlight({0.2f, -1.0f, 1.2f}));

    // spotlight
    // add_light(axgl::Light::spotlight({-0.5f, 2.0f, -2.5f}, {0.2f, -1.0f, 1.2f}));

    // box
    {
      const auto entity = entity_service_->create_entity_t<BoxEntity>();
      entity->set_position(item_position(0.5f));
      add_entity(entity);
    }

    // spinning box
    {
      const auto entity = entity_service_->create_entity_t<BoxEntity>();
      entity->set_position(item_position(1.0f));
      entity->set_rotation_speed({0.02f, 0.01f, 0.05f});
      add_entity(entity);
    }

    // concrete block
    {
      const auto entity = entity_service_->create_entity_t<ConcreteBlockEntity>();
      entity->set_position(item_position(0.5f));
      add_entity(entity);
    }

    // spinning concrete block
    {
      const auto entity = entity_service_->create_entity_t<ConcreteBlockEntity>();
      entity->set_position(item_position(1.0f));
      entity->set_rotation_speed({0.02f, 0.01f, 0.05f});
      add_entity(entity);
    }
  }
};
