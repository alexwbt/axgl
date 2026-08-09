#pragma once

#include <axgl/common.hpp>

#define __AXGL_GUI_STYLE_PROPERTY(type, name, init)                            \
private:                                                                       \
  type name##_ init;                                                           \
  bool using_##name##_ = false;                                                \
                                                                               \
public:                                                                        \
  type get_##name() const                                                      \
  {                                                                            \
    return name##_;                                                            \
  }                                                                            \
  Style* set_##name(const type&(name))                                         \
  {                                                                            \
    name##_ = name;                                                            \
    using_##name##_ = true;                                                    \
    modified_ = true;                                                          \
    return this;                                                               \
  };                                                                           \
  bool using_##name() const                                                    \
  {                                                                            \
    return using_##name##_;                                                    \
  }

#define __AXGL_GUI_STYLE_APPLY_TO(name)                                        \
  if (using_##name##_) target.name##_ = name##_

namespace axgl::gui
{

enum class Display
{
  kBlock,
  // kInline,
  // kInlineBlock,
  // kFlex,
  // kGrid,
};

enum class Cursor
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
  bool modified_ = false;

public:
  [[nodiscard]] bool is_modified() const { return modified_; }
  void reset_modified() { modified_ = false; }

  void apply_to(Style& target, bool mark_updated = true) const
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
    // layout
    __AXGL_GUI_STYLE_APPLY_TO(display);
    __AXGL_GUI_STYLE_APPLY_TO(margin);
    __AXGL_GUI_STYLE_APPLY_TO(padding);
    if (mark_updated) target.modified_ = true;
  }

  __AXGL_GUI_STYLE_PROPERTY(glm::vec2, position, {0.0f})
  __AXGL_GUI_STYLE_PROPERTY(glm::vec2, size, {0.0f})
  __AXGL_GUI_STYLE_PROPERTY(glm::vec4, color, {0.0f})
  __AXGL_GUI_STYLE_PROPERTY(float, opacity, = 1.0f)
  __AXGL_GUI_STYLE_PROPERTY(Cursor, cursor, = Cursor::kNormal)
  // content
  __AXGL_GUI_STYLE_PROPERTY(std::vector<std::string>, fonts, )
  __AXGL_GUI_STYLE_PROPERTY(glm::vec4, font_color, {1.0f})
  __AXGL_GUI_STYLE_PROPERTY(float, font_size, = 16.0f)
  __AXGL_GUI_STYLE_PROPERTY(int, font_weight, = 400)
  __AXGL_GUI_STYLE_PROPERTY(float, line_height, = 1.5f)
  __AXGL_GUI_STYLE_PROPERTY(TextAlign, text_align, = TextAlign::kCenter)
  // layout
  __AXGL_GUI_STYLE_PROPERTY(Display, display, = Display::kBlock)
  __AXGL_GUI_STYLE_PROPERTY(glm::vec4, margin, {0.0f})
  __AXGL_GUI_STYLE_PROPERTY(glm::vec4, padding, {0.0f})
};

} // namespace axgl::gui
