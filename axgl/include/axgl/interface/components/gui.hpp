#pragma once

#include <axgl/interface/component.hpp>
#include <axgl/interface/gui/element.hpp>

namespace axgl::component
{

class Gui : virtual public Component
{
public:
  virtual gui::Element* root() = 0;
};

} // namespace axgl::component
