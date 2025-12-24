#pragma once

#include <axgl/interface/gui/attribute.hpp>
#include <axgl/interface/gui/page.hpp>

namespace axgl::gui
{

class Element
{
public:
  virtual ~Element() = default;
  virtual void render(const axgl::gui::Page::Context& context) { }

  [[nodiscard]] virtual std::uint64_t get_id() const = 0;
  [[nodiscard]] virtual bool should_render() const = 0;
  [[nodiscard]] virtual Attribute& attribute() = 0;
  [[nodiscard]] virtual axgl::Container<axgl::gui::Element>& children() = 0;
};

} // namespace axgl::gui
