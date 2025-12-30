#pragma once

#include <axgl/common.hpp>

#define __AXGL_GUI_PROPERTY(type, name, default_value)                                                                 \
private:                                                                                                               \
  type name##_ = default_value;                                                                                        \
  bool using_##name##_ = false;                                                                                        \
                                                                                                                       \
public:                                                                                                                \
  type get_##name() const                                                                                              \
  {                                                                                                                    \
    if (base_property_ && !using_##name##_)                                                                            \
      return base_property_->get_##name();                                                                             \
    return name##_;                                                                                                    \
  }                                                                                                                    \
  void set_##name(const type&(name))                                                                                   \
  {                                                                                                                    \
    name##_ = name;                                                                                                    \
    using_##name##_ = true;                                                                                            \
  };

namespace axgl::gui
{
//
// enum class TextAlign
// {
//   kLeft,
//   kRight,
//   kCenter,
// };

class Property
{
  Property* base_property_;

public:
  Property() : base_property_(nullptr) { }
  explicit Property(Property* base_property) : base_property_(base_property) { }

  __AXGL_GUI_PROPERTY(glm::vec2, position, glm::vec2(0.0f))
  __AXGL_GUI_PROPERTY(glm::vec2, size, glm::vec2(0.0f))
  __AXGL_GUI_PROPERTY(glm::vec4, color, glm::vec4(0.0f))
  __AXGL_GUI_PROPERTY(float, opacity, 1.0f)
  // layout
  // glm::vec4 margin{0.0f};
  // glm::vec4 padding{0.0f};
  // content
  // std::string content;
  // std::string font;
  // glm::vec4 font_color{1.0f};
  // float font_size = 16.0f;
  // float line_height = 1.5f;
  // TextAlign text_align = TextAlign::kCenter;
};

} // namespace axgl::gui
