#pragma once

#include <functional>
#include <string>
#include <vector>

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>

namespace axgl::gui {

class DropdownElement : virtual public axgl::gui::TextElement {
public:
  using Handler = std::function<void(const axgl::gui::Context&, std::size_t)>;

  static constexpr std::string_view kTypeId = "element:dropdown";

  virtual void set_options(const std::vector<std::string>& options) = 0;
  [[nodiscard]] virtual const std::vector<std::string>& get_options() const = 0;
  virtual void set_selected(std::size_t index) = 0;
  [[nodiscard]] virtual std::size_t get_selected() const = 0;

  virtual void on_change(Handler handler) = 0;
};

} // namespace axgl::gui
