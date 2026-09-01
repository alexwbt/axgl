#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>

namespace axgl::gui
{

class ButtonElement : virtual public axgl::gui::Element
{
public:
  static constexpr std::string_view kTypeId = "element:button";

  virtual void set_text(const std::string& text) = 0;
  [[nodiscard]] virtual std::string get_text() const = 0;

  [[nodiscard]] virtual TextElement& label() const = 0;

  virtual void on_click(std::function<void(const axgl::gui::Context&)> handler)
    = 0;
};

} // namespace axgl::gui
