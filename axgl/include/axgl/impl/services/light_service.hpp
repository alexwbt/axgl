#pragma once

#include <algorithm>
#include <span>
#include <vector>

#include <axgl/interface/light.hpp>
#include <axgl/interface/services/light_service.hpp>

#include <axgl/impl/components/light.hpp>
#include <axgl/impl/service_base.hpp>

namespace axgl::impl
{

class LightService : virtual public axgl::LightService, public impl::ServiceBase
{
  std::vector<const axgl::Light*> lights_;

public:
  void initialize() override { get_context()->axgl->entity_service()->register_component_t<component::Light>(); }

  void add_light(const Light* light) override { lights_.push_back(light); }
  void remove_light(const Light* light) override { lights_.erase(std::ranges::find(lights_, light)); }
  void clear_lights() override { lights_.clear(); }
  std::span<const axgl::Light*> get_lights() override { return lights_; }
};

} // namespace axgl::impl
