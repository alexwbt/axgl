#pragma once

#include <axgl/interface/light.hpp>
#include <axgl/interface/services/light_service.hpp>

#include <axgl/impl/component_base.hpp>

namespace axgl::impl::component
{

class Light : public ComponentBase
{
public:
  static constexpr std::string_view kType = "component::light";

private:
  std::shared_ptr<axgl::LightService> light_service_;

public:
  axgl::Light light;

  void on_create() override
  {
    light_service_ = get_entity()->get_context()->axgl->light_service();
    light_service_->add_light(&light);
  }

  void on_remove() override { light_service_->remove_light(&light); }

  void update() override
  {
    const auto transform = get_entity()->transform();
    light.position = transform->position;
    light.direction = transform->rotation;
  }
};

} // namespace axgl::impl::component
