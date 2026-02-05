#pragma once

#include "common_scene.hpp"

#include "entity/box.hpp"

class LightScene : public CommonScene
{
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
      auto box = entity_service_->create_entity_t<BoxEntity>();
      box->transform().position = {1.0f, 0.5f, 0.0f};
      box->update_model_matrix();
      entities_.add(std::move(box));
    }

    // spinning box
    {
      auto box = entity_service_->create_entity_t<BoxEntity>();
      box->transform().position = {5.0f, 1.0f, 0.0f};
      box->update_model_matrix();
      box->set_rotation_speed({0.02f, 0.01f, 0.05f});
      entities_.add(std::move(box));
    }
  }
};
