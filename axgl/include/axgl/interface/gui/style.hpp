#pragma once

#include <axgl/common.hpp>

#define __AXGL_GUI_STYLE_PROPERTY(type, name, init)                                                                    \
private:                                                                                                               \
  type name##_ init;                                                                                                   \
  bool using_##name##_ = false;                                                                                        \
                                                                                                                       \
public:                                                                                                                \
  type get_##name() const                                                                                              \
  {                                                                                                                    \
    return name##_;                                                                                                    \
  }                                                                                                                    \
  Style* set_##name(const type&(name))                                                                                 \
  {                                                                                                                    \
    name##_ = name;                                                                                                    \
    using_##name##_ = true;                                                                                            \
    updated_ = true;                                                                                                   \
    return this;                                                                                                       \
  };                                                                                                                   \
  bool using_##name() const                                                                                            \
  {                                                                                                                    \
    return using_##name##_;                                                                                            \
  }

#define __AXGL_GUI_STYLE_APPLY_TO(name)                                                                                \
  if (using_##name##_) target.set_##name(name##_)

namespace axgl::gui
{

enum class CursorType
{
  kNormal,
  kText,
  kPointer,
  kCrosshair,
  kResizeVertical,
  kResizeHorizontal,
  kResizeDiagonalLeft,
  kResizeDiagonalRight,
  kResize,
  kNotAllowed,
};

enum class TextAlign
{
  kLeft,
  kRight,
  kCenter,
};

class Style
{
private:
  bool updated_ = false;

public:
  [[nodiscard]] bool updated() const { return updated_; }
  void reset_updated() { updated_ = false; }

  void apply_to(Style& target)
  {
    __AXGL_GUI_STYLE_APPLY_TO(position);
    __AXGL_GUI_STYLE_APPLY_TO(size);
    __AXGL_GUI_STYLE_APPLY_TO(color);
    __AXGL_GUI_STYLE_APPLY_TO(opacity);
    __AXGL_GUI_STYLE_APPLY_TO(cursor);
    // content
    __AXGL_GUI_STYLE_APPLY_TO(fonts);
    __AXGL_GUI_STYLE_APPLY_TO(font_color);
    __AXGL_GUI_STYLE_APPLY_TO(font_size);
    __AXGL_GUI_STYLE_APPLY_TO(line_height);
    __AXGL_GUI_STYLE_APPLY_TO(text_align);
  }

  __AXGL_GUI_STYLE_PROPERTY(glm::vec2, position, {0.0f})
  __AXGL_GUI_STYLE_PROPERTY(glm::vec2, size, {0.0f})
  __AXGL_GUI_STYLE_PROPERTY(glm::vec4, color, {0.0f})
  __AXGL_GUI_STYLE_PROPERTY(float, opacity, = 1.0f)
  __AXGL_GUI_STYLE_PROPERTY(CursorType, cursor, = CursorType::kNormal)
  // content
  __AXGL_GUI_STYLE_PROPERTY(std::vector<std::string>, fonts, );
  __AXGL_GUI_STYLE_PROPERTY(glm::vec4, font_color, {1.0f})
  __AXGL_GUI_STYLE_PROPERTY(float, font_size, = 16.0f)
  __AXGL_GUI_STYLE_PROPERTY(float, line_height, = 1.5f)
  __AXGL_GUI_STYLE_PROPERTY(TextAlign, text_align, = TextAlign::kCenter)
  // layout
  // __AXGL_GUI_STYLE_PROPERTY(glm::vec4, margin, {0.0f})
  // __AXGL_GUI_STYLE_PROPERTY(glm::vec4, padding, {0.0f})
};

} // namespace axgl::gui
