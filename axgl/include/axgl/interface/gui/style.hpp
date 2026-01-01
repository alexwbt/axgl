#pragma once

#include <algorithm>
#include <ranges>

#include <axgl/common.hpp>

#define __AXGL_GUI_STYLE_PROPERTY(type, name, init)                                                                    \
private:                                                                                                               \
  type name##_ init;                                                                                                   \
  bool using_##name##_ = false;                                                                                        \
                                                                                                                       \
public:                                                                                                                \
  type get_##name() const                                                                                              \
  {                                                                                                                    \
    if (using_##name##_) return name##_;                                                                               \
    for (const auto& style : std::ranges::views::reverse(shared_styles_))                                              \
      if (const auto style_ptr = style.lock(); style_ptr && style_ptr->using_##name()) return style_ptr->get_##name(); \
    return name##_;                                                                                                    \
  }                                                                                                                    \
  Style* set_##name(const type&(name))                                                                                 \
  {                                                                                                                    \
    name##_ = name;                                                                                                    \
    using_##name##_ = true;                                                                                            \
    return this;                                                                                                       \
  };                                                                                                                   \
  bool using_##name() const                                                                                            \
  {                                                                                                                    \
    return using_##name##_                                                                                             \
      || std::ranges::any_of(                                                                                          \
             shared_styles_, [](const auto& e)                                                                         \
    {                                                                                                                  \
      const auto style_ptr = e.lock();                                                                                 \
      return style_ptr && style_ptr->using_##name();                                                                   \
    });                                                                                                                \
  }

namespace axgl::gui
{

enum class CursorType
{
  kNormal,
  kText,
  kPointer,
  kCrosshair,
  kResizeNS,
  kResizeEW,
  kResizeNESW,
  kResizeNWSE,
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
  std::vector<axgl::ref_t<Style>> shared_styles_;

public:
  Style* with(const axgl::ptr_t<Style>& shared_style)
  {
    shared_styles_.emplace_back(shared_style);
    return this;
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
