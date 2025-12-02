#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/light.hpp>

#include <axgl/impl/component_base.hpp>

namespace axgl::impl::component
{

class Light : public ComponentBase
{
public:
  static constexpr std::string_view kTypeId = "component::light";

  axgl::Light light;

  void on_entity_create(const axgl::Entity::Context& context) override { context.lights.push_back(&light); }

  void on_entity_remove(const axgl::Entity::Context& context) override { std::erase(context.lights, &light); }

  void update(const axgl::Entity::Context& context) override
  {
    const auto& transform = context.entity.transform();
    light.position = transform.position;
    light.direction = transform.rotation;
  }
};

} // namespace axgl::impl::component
