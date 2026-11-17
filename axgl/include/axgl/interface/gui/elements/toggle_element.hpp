#pragma once

#include <functional>

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>

namespace axgl::gui {

class ToggleElement : virtual public axgl::gui::TextElement {
public:
  using Handler = std::function<void(const axgl::gui::Context&, bool)>;

  static constexpr std::string_view kTypeId = "element:toggle";

  virtual void set_checked(bool checked) = 0;
  [[nodiscard]] virtual bool is_checked() const = 0;

  virtual void on_toggle(Handler handler) = 0;
};

} // namespace axgl::gui
