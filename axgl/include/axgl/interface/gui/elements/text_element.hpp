#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/element.hpp>

namespace axgl::gui
{

class TextElement : virtual public axgl::gui::Element
{
public:
  static constexpr std::string_view kTypeId = "element:text";

  virtual void set_text(const std::string& text) = 0;
  [[nodiscard]] virtual std::string get_text() const = 0;
};

} // namespace axgl::gui
