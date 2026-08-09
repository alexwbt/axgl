#pragma once

#include <axgl/interface/container.hpp>
#include <axgl/interface/gui/context.hpp>

namespace axgl::gui
{

class Element;

class LayoutContext : axgl::gui::Context
{
};

class Layout
{
public:
  virtual ~Layout() = default;

  virtual void apply(axgl::Container<axgl::gui::Element>& element) const = 0;
};

} // namespace axgl::gui
