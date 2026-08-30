#pragma once

#include "common_scene.hpp"
#include "entity/concrete_block.hpp"

class SpotLightScene : public CommonScene
{
public:
  static constexpr std::string_view kTypeId = "realm::spot-light";

private:
  float rotation_ = 0.0f;
  glm::vec3 position_{2.0f, 2.0f, 1.0f};
  axgl::ptr_t<axgl::impl::component::Light> spot_light_;

public:
  void initialize() override
  {
    CommonScene::initialize();

    // spot light
    spot_light_ = add_light(axgl::Light::spotlight(position_, -position_));
    add_entity<ConcreteBlockEntity>({0.0f, 0.5f, 0.0f});
  }

  void update() override
  {
    CommonScene::update();

    if (input_manager_->rotate_sun()->down())
    {
      rotation_ += context_->delta_tick_f * 0.1f;
      float x = std::cos(rotation_);
      float y = std::sin(rotation_);
      const glm::vec3 r{x, 0.0f, y};

      auto& light = spot_light_->light;
      light.position = r + position_;
      light.direction = r - light.position;
    }
  }
};
