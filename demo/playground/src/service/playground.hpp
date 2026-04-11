#pragma once

#include <axgl/common.hpp>

#include <axgl/impl/service_base.hpp>

#include "scene/testing_scene.hpp"

#include "scene/component/spinning.hpp"

#include "scene/entity/box.hpp"
#include "scene/entity/bunny.hpp"
#include "scene/entity/camera.hpp"
#include "scene/entity/concrete_block.hpp"
#include "scene/entity/cube.hpp"
#include "scene/entity/debug_axes_indicator.hpp"
#include "scene/entity/floor.hpp"
#include "scene/entity/toy_block.hpp"

class Playground : public axgl::impl::ServiceBase
{
public:
  static constexpr std::string_view kTypeId = "service::playground";

private:
  std::vector<axgl::ptr_t<axgl::Realm>> scenes_;

public:
  void initialize() override
  {
    const auto& entity_service = axgl_->entity_service();
    const auto& realm_service = axgl_->realm_service();
    // register components
    entity_service->register_component_t<SpinningComponent>();
    // register entities
    entity_service->register_entity_t<BoxEntity>();
    entity_service->register_entity_t<BunnyEntity>();
    entity_service->register_entity_t<CameraEntity>();
    entity_service->register_entity_t<ConcreteBlockEntity>();
    entity_service->register_entity_t<CubeEntity>();
    entity_service->register_entity_t<DebugAxesIndicatorEntity>();
    entity_service->register_entity_t<FloorEntity>();
    entity_service->register_entity_t<ToyBlockEntity>();
    // register scenes
    realm_service->register_realm_t<TestingScene>();
  }

  void on_start() override
  {
    const auto& realm_service = axgl_->realm_service();
    // create scenes
    scenes_.emplace_back(realm_service->create_realm_t<TestingScene>());

    // set first scene as active realm
    axgl_->realm_service()->set_active_realm(scenes_[0]);
  }
};
