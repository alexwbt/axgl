#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/camera.hpp>

#include <axgl/impl/component_base.hpp>

namespace axgl::impl::component
{

class Camera : public axgl::impl::ComponentBase
{
public:
  static constexpr std::string_view kTypeId = "component::camera";
  axgl::Camera camera;
};

} // namespace axgl::impl::component
