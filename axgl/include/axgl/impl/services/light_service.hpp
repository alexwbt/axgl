#pragma once

#include <algorithm>
#include <span>
#include <vector>

#include <axgl/interface/light.hpp>
#include <axgl/interface/services/light_service.hpp>

namespace axgl::impl
{

class LightService : virtual public axgl::LightService
{
  std::vector<const axgl::Light*> lights_;

public:
  void add_light(const Light* light) override { lights_.push_back(light); }
  void remove_light(const Light* light) override { lights_.erase(std::ranges::find(lights_, light)); }
  void clear_lights() override { lights_.clear(); }
  std::span<const axgl::Light*> get_lights() override { return lights_; }
};

} // namespace axgl::impl
