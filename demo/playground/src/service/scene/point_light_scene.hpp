#pragma once

#include "common_scene.hpp"
#include "entity/concrete_block.hpp"

class PointLightScene : public CommonScene
{
public:
  static constexpr std::string_view kTypeId = "realm::point-light";

private:
  float rotation_ = 0.0f;
  axgl::ptr_t<axgl::impl::component::Light> point_light_;

public:
  void on_create() override
  {
    CommonScene::on_create();

    // point light
    point_light_ = add_light(axgl::Light::point_light({2.0f, 2.0f, 0.0f}));
    add_entity<ConcreteBlockEntity>({0.0f, 0.5f, 0.0f});
  }

  void update() override
  {
    CommonScene::update();

    if (input_manager_->rotate_sun()->down())
    {
      rotation_ += context_->delta_tick_f * 0.1f;
      float x = std::cos(rotation_) * 2.0f;
      float y = std::sin(rotation_) * 2.0f;
      point_light_->light.position = {x, 2.0f, y};
    }
  }
};
