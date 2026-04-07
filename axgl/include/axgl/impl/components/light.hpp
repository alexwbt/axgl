#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/light.hpp>

#include <axgl/impl/component_base.hpp>
#include <axgl/impl/components/transform.hpp>

namespace axgl::impl::component
{

class Light : public ComponentBase
{
public:
  static constexpr std::string_view kTypeId = "component::light";

  axgl::Light light{axgl::Light::Type::kSun};

  void update() override
  {
    const auto& transform = parent_->get_component_t<component::Transform>();
    light.position = transform.position;
  }
};

} // namespace axgl::impl::component
