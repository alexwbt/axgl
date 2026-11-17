#pragma once

#include <functional>

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>

namespace axgl::gui {

class SliderElement : virtual public axgl::gui::Element {
public:
  using Handler = std::function<void(const axgl::gui::Context&, float)>;

  static constexpr std::string_view kTypeId = "element:slider";

  virtual void set_value(float value) = 0;
  [[nodiscard]] virtual float get_value() const = 0;
  virtual void set_range(float min, float max) = 0;

  virtual void on_change(Handler handler) = 0;
};

} // namespace axgl::gui
