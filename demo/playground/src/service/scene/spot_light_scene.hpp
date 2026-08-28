#pragma once

#include "common_scene.hpp"
#include "entity/concrete_block.hpp"

class SpotLightScene : public CommonScene
{
public:
  static constexpr std::string_view kTypeId = "realm::spot-light";

public:
  void initialize() override
  {
    CommonScene::initialize();

    // spot light
    glm::vec3 light_position{2.0f, 2.0f, 0.0f};
    add_light(axgl::Light::spotlight(light_position, -light_position));
    add_entity<ConcreteBlockEntity>({0.0f, 0.5f, 0.0f});
  }
};
