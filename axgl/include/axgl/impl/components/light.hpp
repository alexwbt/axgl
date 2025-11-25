#pragma once

#include <axgl/interface/light.hpp>

#include <axgl/impl/component_base.hpp>

namespace axgl::impl::component
{

class Light : public ComponentBase
{
public:
  axgl::Light light;

  void update() override
  {
    const auto transform = get_entity()->transform();
    light.position = transform->position;
    light.direction = transform->rotation;

    const auto context = get_entity()->get_context();
    context->lights.push_back(&light);
  }
};

} // namespace axgl::impl::component
