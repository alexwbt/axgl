#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>

#include "scene/testing_scene.hpp"

#include "scene/entity/box.hpp"
#include "scene/entity/camera.hpp"
#include "scene/entity/concrete_block.hpp"
#include "scene/entity/cube.hpp"
#include "scene/entity/debug_cursor.hpp"
#include "scene/entity/floor.hpp"

class Playground : public axgl::Service
{
  std::vector<axgl::ptr_t<axgl::Realm>> scenes_;

public:
  void initialize(const Context& context) override
  {
    const auto& axgl = context.axgl;
    const auto& entity_service = axgl.entity_service();
    // register entities
    entity_service->register_entity_t<BoxEntity>();
    entity_service->register_entity_t<CameraEntity>();
    entity_service->register_entity_t<ConcreteBlockEntity>();
    entity_service->register_entity_t<CubeEntity>();
    entity_service->register_entity_t<DebugCursorEntity>();
    entity_service->register_entity_t<FloorEntity>();
  }

  void on_start(const Context& context) override
  {
    // create scenes
    scenes_.emplace_back(axgl::create_ptr<TestingScene>());

    // initialize scenes
    for (const auto& scene : scenes_)
      scene->initialize(context);

    // set first scene as active realm
    context.axgl.realm_service()->set_active_realm(scenes_[0]);
  }
};
