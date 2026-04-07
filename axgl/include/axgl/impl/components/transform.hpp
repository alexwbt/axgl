#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/transform.hpp>

#include <axgl/impl/component_base.hpp>

namespace axgl::impl::component
{

class Transform : public axgl::impl::ComponentBase
{
public:
  static constexpr std::string_view kTypeId = "component::transform";
  axgl::Transform transform;
};

} // namespace axgl::impl::component
