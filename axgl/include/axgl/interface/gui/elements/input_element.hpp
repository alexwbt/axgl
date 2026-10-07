#pragma once

#include <functional>

#include <axgl/common.hpp>
#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>

namespace axgl::gui {

class InputElement : virtual public axgl::gui::TextElement {
public:
  static constexpr std::string_view kTypeId = "element:input";

  virtual void on_submit(std::function<void(const axgl::gui::Context&)> handler)
    = 0;
};

} // namespace axgl::gui
