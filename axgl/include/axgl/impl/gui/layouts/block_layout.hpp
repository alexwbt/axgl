#pragma once

#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/gui/layout.hpp>

namespace axgl::impl::gui
{

class BlockLayout : virtual public axgl::gui::Layout
{
public:
  void apply(glm::ivec2 size, axgl::Container<axgl::gui::Element>& elements)
    const override
  {
    using namespace axgl::gui;

    int x = 0, y = 0;

    for (const auto& element : elements.get())
    {
      const auto& style = element->style();

      switch (style->get_display())
      {
      case Display::kBlock: element->set_size(size.x, intrinsic_height); break;
      case Display::kInline:
      }
    }
  }
};

} // namespace axgl::impl::gui
