#pragma once

#include <axgl/interface/container.hpp>

namespace axgl::gui
{

class Element;

class Layout
{
public:
  virtual ~Layout() = default;

  // apply layout from root to leaf
  virtual void apply(
    glm::ivec2 size,
    axgl::Container<axgl::gui::Element>& element
  ) const
    = 0;
};

} // namespace axgl::gui
