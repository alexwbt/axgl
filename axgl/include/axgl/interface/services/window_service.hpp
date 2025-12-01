#pragma once

#include <string_view>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/window.hpp>

namespace axgl
{

class WindowService : virtual public axgl::Service
{
public:
  static constexpr std::string_view kTypeId = "service::window";

  virtual axgl::ptr_t<axgl::Window> create_window() = 0;
};

} // namespace axgl
