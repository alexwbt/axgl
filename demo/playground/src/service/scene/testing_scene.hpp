#pragma once

#include "common_scene.hpp"

#include "entity/box.hpp"
#include "entity/concrete_block.hpp"
#include "entity/cube.hpp"

class TestingScene : public CommonScene
{
  glm::vec2 item_offset{0.0f};
  float margin = 2.0f;

  axgl::ptr_t<axgl::impl::component::Light> sunlight_;

  void next_row()
  {
    item_offset.y += margin * 2;
    item_offset.x = 0;
  }

  glm::vec3 item_position(float height)
  {
    item_offset.x += margin * 2.0f;
    return {item_offset.x - margin, height, item_offset.y};
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
    sunlight_ = add_light(axgl::Light::sunlight({0.2f, -1.0f, 1.2f}));

    // spotlight
    // add_light(axgl::Light::spotlight({-0.5f, 2.0f, -2.5f}, {0.2f, -1.0f, 1.2f}));

    // cube
    {
      const auto entity = entity_service_->create_entity_t<CubeEntity>();
      entity->set_position(item_position(0.5f));
      add_entity(entity);
    }
    // spinning cube
    {
      const auto entity = entity_service_->create_entity_t<CubeEntity>();
      entity->set_position(item_position(1.0f));
      entity->set_rotation_speed({0.02f, 0.01f, 0.05f});
      add_entity(entity);
    }
    next_row();

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
    next_row();

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
    next_row();
  }

  void update(const axgl::Service::Context& context) override
  {
    CommonScene::update(context);

    if (inputs_->rotate_sun()->tick > 0)
    {
      sunlight_->light.direction.z -= 0.05f * context.delta_tick_f;
      if (sunlight_->light.direction.z < -1.2f) sunlight_->light.direction.z = 1.2f;
    }
  }
};
