#pragma once

#include "common_scene.hpp"

#include "component/spinning.hpp"

#include "entity/box.hpp"
#include "entity/bunny.hpp"
#include "entity/concrete_block.hpp"
#include "entity/cube.hpp"
#include "entity/toy_block.hpp"
#include "entity/transparent_cube.hpp"

class TestingScene : public CommonScene
{
public:
  static constexpr std::string_view kTypeId = "realm::testing";

private:
  glm::vec2 item_offset{0.0f};
  float margin = 2.0f;

  axgl::ptr_t<axgl::impl::component::Light> sunlight_;

public:
  void initialize() override
  {
    CommonScene::initialize();

    // sunlight
    sunlight_ = add_light(axgl::Light::sunlight({0.2f, -1.0f, 1.2f}));

    // spinning component
    axgl::ptr_t<axgl::Component> spinning_comp;
    {
      const auto component
        = entity_service_->create_component_t<SpinningComponent>();
      component->set_rotation_speed({0.02f, 0.01f, 0.05f});
      spinning_comp = axgl::ptr_cast<axgl::Component>(component);
    }

    // cube
    add_entity<CubeEntity>(item_position(0.5f));
    add_entity<CubeEntity>(item_position(1.0f))->add_component(spinning_comp);
    next_row();

    // transparent cube
    add_entity<TransparentCubeEntity>(item_position(0.5f));
    add_entity<TransparentCubeEntity>(item_position(1.0f))
      ->add_component(spinning_comp);
    next_row();

    // box
    add_entity<BoxEntity>(item_position(0.5f));
    add_entity<BoxEntity>(item_position(1.0f))->add_component(spinning_comp);
    next_row();

    // concrete block
    add_entity<ConcreteBlockEntity>(item_position(0.5f));
    add_entity<ConcreteBlockEntity>(item_position(1.0f))
      ->add_component(spinning_comp);
    next_row();

    // toy block
    add_entity<ToyBlockEntity>(item_position(0.5f));
    add_entity<ToyBlockEntity>(item_position(1.0f))
      ->add_component(spinning_comp);
    next_row();

    // bunny
    add_entity<BunnyEntity>(item_position(0.5f));
    add_entity<BunnyEntity>(item_position(1.0f))->add_component(spinning_comp);
    next_row();
  }

  void update() override
  {
    CommonScene::update();

    if (input_manager_->rotate_sun()->tick > 0)
    {
      sunlight_->light.direction.z -= 0.05f * context_->delta_tick_f;
      if (sunlight_->light.direction.z < -1.2f)
        sunlight_->light.direction.z = 1.2f;
    }
  }

private:
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
};
