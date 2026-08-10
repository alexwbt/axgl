#pragma once

#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/gui/layout.hpp>

namespace axgl::impl::gui
{

class BlockLayout : virtual public axgl::gui::Layout
{
public:
  void apply(
    glm::ivec2 size,
    axgl::Container<axgl::gui::Element>& elements
  ) const override
  {
    using namespace axgl::gui;

    float x = 0.0f, y = 0.0f;

    for (const auto& element : elements.get())
    {
      const auto& style = element->get_computed_style();
      const auto& margin = style.get_margin();
      const auto& intrinsic_size = element->get_intrinsic_size();

      switch (style.get_display())
      {
      case Display::kBlock:
        y += margin.x;
        x += margin.z;
        element->set_position({x, y});
        element->set_size({size.x, intrinsic_size.y});
        x = 0.0f;
        y += margin.y + intrinsic_size.y;
        break;
      case Display::kInline: break;
      }
    }
  }
};

} // namespace axgl::impl::gui
