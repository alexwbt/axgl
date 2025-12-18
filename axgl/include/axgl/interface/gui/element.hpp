#pragma once

#include <axgl/interface/container.hpp>

#include <axgl/interface/gui/attribute.hpp>

namespace axgl
{
class Axgl;
class GuiService;
} // namespace axgl

namespace axgl::gui
{

class Element
{
public:
  struct Context
  {
    axgl::Axgl& axgl;
    axgl::GuiService& gui_service;
    Element* parent;
  };

  virtual ~Element() = default;
  virtual void render(const Context& context) { }

  [[nodiscard]] virtual std::uint64_t get_id() const = 0;
  [[nodiscard]] virtual Attribute& attribute() = 0;

  [[nodiscard]] virtual axgl::Container<axgl::gui::Element>& children() = 0;
};

} // namespace axgl::gui
