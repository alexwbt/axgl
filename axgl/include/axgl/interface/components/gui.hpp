#pragma once

#include <string_view>

#include <axgl/interface/component.hpp>
#include <axgl/interface/gui/element.hpp>

namespace axgl::component
{

class Gui : virtual public Component
{
public:
  static constexpr std::string_view kTypeId = "component::gui";

  virtual std::shared_ptr<axgl::gui::Element> root() = 0;
  virtual void set_root(std::shared_ptr<axgl::gui::Element> root) = 0;
};

} // namespace axgl::component
