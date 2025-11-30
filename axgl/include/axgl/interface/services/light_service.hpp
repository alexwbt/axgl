#pragma once

#include <span>
#include <string_view>

#include <axgl/interface/light.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class LightService : virtual public Service
{
public:
  static constexpr std::string_view kTypeId = "service::light";

  virtual void add_light(const Light* light) = 0;
  virtual void remove_light(const Light* light) = 0;
  virtual void clear_lights() = 0;
  virtual std::span<const Light*> get_lights() = 0;
};

} // namespace axgl
