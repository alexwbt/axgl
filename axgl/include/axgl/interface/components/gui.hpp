#pragma once

#include <string_view>

#include <axgl/common.hpp>
#include <axgl/interface/component.hpp>
#include <axgl/interface/gui/element.hpp>

namespace axgl::component
{

class Gui : virtual public Component
{
public:
  static constexpr std::string_view kTypeId = "component::gui";

  virtual ptr_t<axgl::gui::Element> root() = 0;
  virtual void set_root(ptr_t<axgl::gui::Element> root) = 0;
};

} // namespace axgl::component
