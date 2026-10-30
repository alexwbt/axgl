#pragma once

#include <functional>

#include <axgl/common.hpp>
#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>

namespace axgl::gui {

class InputElement : virtual public axgl::gui::TextElement {
public:
  using Handler
    = std::function<void(const axgl::gui::Context&, const std::string&)>;

  static constexpr std::string_view kTypeId = "element:input";

  virtual void on_change(Handler handler) = 0;
  virtual void on_submit(Handler handler) = 0;
};

} // namespace axgl::gui
