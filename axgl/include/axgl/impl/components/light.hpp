#pragma once

#include <axgl/interface/light.hpp>

#include <axgl/impl/component_base.hpp>

namespace axgl::impl::component
{

class Light : public ComponentBase
{
public:
  static constexpr std::string_view kTypeId = "component::light";

  axgl::Light light{axgl::Light::Type::kSun};

  void update() override
  {
    const auto& transform = context_.entity.transform();
    light.position = transform.position;
  }
};

} // namespace axgl::impl::component
