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

  axgl::Light light{axgl::Light::Type::kSun};

  void parent_update(axgl::Entity* parent) override { light.position = parent->transform().position; }
};

} // namespace axgl::impl::component
