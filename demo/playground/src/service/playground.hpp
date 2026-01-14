#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>

#include "../entity/box.hpp"
#include "../entity/debug_cursor.hpp"
#include "../entity/floor.hpp"
#include "../scene/blinn_scene.hpp"

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
    entity_service->register_entity_t<FloorEntity>();
    entity_service->register_entity_t<DebugCursorEntity>();
  }

  void on_start(const Context& context) override
  {
    const auto& axgl = context.axgl;
    const auto& realm_service = axgl.realm_service();

    // create scenes
    scenes_.emplace_back(blinn_scene(axgl));

    // set first scene as active realm
    realm_service->set_active_realm(scenes_[0]);
  }
};
