#pragma once

#include <axgl/interface/container.hpp>
#include <axgl/interface/gui/context.hpp>

namespace axgl::gui {

class Element;

class Layout {
public:
  virtual ~Layout() = default;

  virtual glm::vec2 measure(
    const axgl::gui::Context& context,
    axgl::Container<axgl::gui::Element>& elements,
    const glm::vec2& available_size
  ) const = 0;

  virtual void arrange(
    const axgl::gui::Context& context,
    axgl::Container<axgl::gui::Element>& elements,
    const glm::vec2& content_origin,
    const glm::vec2& available_size
  ) const = 0;
};

} // namespace axgl::gui
