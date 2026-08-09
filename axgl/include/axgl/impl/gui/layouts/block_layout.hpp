#pragma once

#include <axgl/interface/gui/layout.hpp>

namespace axgl::impl::gui
{

class BlockLayout : virtual public axgl::gui::Layout
{
public:
  void apply(glm::ivec2 size, axgl::Container<axgl::gui::Element>& element)
    const override
  {
  }
};

} // namespace axgl::impl::gui
