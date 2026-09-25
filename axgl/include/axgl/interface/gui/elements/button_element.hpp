#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>

namespace axgl::gui {

class ButtonElement : virtual public axgl::gui::Element,
                      virtual public axgl::gui::TextElement {
public:
  static constexpr std::string_view kTypeId = "element:button";

  virtual void on_click(std::function<void(const axgl::gui::Context&)> handler)
    = 0;
};

} // namespace axgl::gui
