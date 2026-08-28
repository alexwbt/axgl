#pragma once

#include "common_scene.hpp"
#include "entity/concrete_block.hpp"

class PointLightScene : public CommonScene
{
public:
  static constexpr std::string_view kTypeId = "realm::point-light";

public:
  void initialize() override
  {
    CommonScene::initialize();

    // point light
    add_light(axgl::Light::point_light({2.0f, 2.0f, 0.0f}));
    add_entity<ConcreteBlockEntity>({0.0f, 0.5f, 0.0f});
  }
};
