#pragma once

#include <axgl/common.hpp>

#include <axgl/impl/service_base.hpp>

#include "scene/entity/transparent_cube.hpp"
#include "scene/hdr_scene.hpp"
#include "scene/point_light_scene.hpp"
#include "scene/spot_light_scene.hpp"
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
  std::size_t current_scene_ = 0;
  std::vector<axgl::ptr_t<axgl::Realm>> scenes_;

  axgl::ptr_t<InputManager> inputs_;

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
    entity_service->register_entity_t<TransparentCubeEntity>();
    // register scenes
    realm_service->register_realm_t<TestingScene>();
    realm_service->register_realm_t<SpotLightScene>();
    realm_service->register_realm_t<PointLightScene>();
    realm_service->register_realm_t<HdrScene>();
  }

  void on_start() override
  {
    inputs_ = axgl_->get_service_t<InputManager>();

    const auto& realm_service = axgl_->realm_service();
    // create scenes
    scenes_.emplace_back(realm_service->create_realm_t<TestingScene>());
    scenes_.emplace_back(realm_service->create_realm_t<SpotLightScene>());
    scenes_.emplace_back(realm_service->create_realm_t<PointLightScene>());
    scenes_.emplace_back(realm_service->create_realm_t<HdrScene>());

    // set first scene as active realm
    axgl_->realm_service()->set_active_realm(scenes_[current_scene_]);
  }

  void update() override
  {
    // switch scenes
    if (inputs_->switch_scene()->clicked())
    {
      current_scene_ = (current_scene_ + 1) % scenes_.size();
      axgl_->realm_service()->set_active_realm(scenes_[current_scene_]);
    }

    // renderer inputs
    const auto& renderer = axgl_->renderer_service()->get_active_renderer();
    // blend
    if (inputs_->blend()->clicked())
      renderer->set_enable_blend(!renderer->get_enable_blend());
    // msaa
    if (inputs_->msaa()->clicked())
      renderer->set_enable_msaa(!renderer->get_enable_msaa());
    // shadow
    if (inputs_->shadow()->clicked())
      renderer->set_enable_shadow(!renderer->get_enable_shadow());
    if (inputs_->debug_csm()->clicked())
    {
      const auto& gl_renderer = axgl::ptr_cast<axgl::impl::opengl::Renderer>(
        axgl_->renderer_service()->get_active_renderer()
      );
      if (gl_renderer)
        gl_renderer->set_enable_csm_debug(!gl_renderer->get_enable_csm_debug());
    }
    // hdr
    if (inputs_->hdr()->clicked())
      renderer->set_enable_hdr(!renderer->get_enable_hdr());
    if (inputs_->exposure_up()->down())
      renderer->set_exposure(renderer->get_exposure() + 0.01f);
    if (inputs_->exposure_down()->down())
      renderer->set_exposure(std::max(0.01f, renderer->get_exposure() - 0.01f));
    // ssao
    if (inputs_->ssao()->clicked())
      renderer->set_enable_ssao(!renderer->get_enable_ssao());
    // bloom
    if (inputs_->bloom()->clicked())
      renderer->set_enable_bloom(!renderer->get_enable_bloom());
  }
};
