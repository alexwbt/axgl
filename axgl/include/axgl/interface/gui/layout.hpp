#pragma once

#include <axgl/interface/container.hpp>
#include <axgl/interface/gui/context.hpp>

namespace axgl::gui
{

class Element;

class Layout
{
public:
  virtual ~Layout() = default;

  virtual void apply(
    const axgl::gui::Context& context,
    axgl::Container<axgl::gui::Element>& element
  ) const
    = 0;
};

} // namespace axgl::gui
