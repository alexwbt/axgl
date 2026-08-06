#pragma once

#include <axgl/interface/gui/element.hpp>

namespace axgl::gui
{

class Layout
{
public:
  virtual ~Layout() = default;

  virtual glm::vec2 get_elemnt_intrinsic_size(
    axgl::gui::Element& element, glm::vec2 available_size) const
    = 0;

  virtual void layout(axgl::gui::Element& element, glm::vec4 content_box) const
    = 0;
};

} // namespace axgl::gui
