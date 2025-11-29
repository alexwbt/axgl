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

  void on_entity_create(const Entity::Context& context) override
  {
    light_service_ = context.axgl.light_service();
    light_service_->add_light(&light);
  }

  void on_entity_remove(const Entity::Context& context) override { light_service_->remove_light(&light); }

  void update(const Entity::Context& context) override
  {
    const auto& transform = context.entity.transform();
    light.position = transform.position;
    light.direction = transform.rotation;
  }
};

} // namespace axgl::impl::component
