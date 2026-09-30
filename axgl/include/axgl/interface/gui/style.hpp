#pragma once

#include <axgl/common.hpp>

#define __AXGL_GUI_STYLE_PROPERTY(type, name, init)                            \
private:                                                                       \
  type name##_ init;                                                           \
  bool using_##name##_ = false;                                                \
                                                                               \
public:                                                                        \
  type get_##name() const {                                                    \
    if (!using_##name##_ && base_style_) return base_style_->get_##name();     \
    return name##_;                                                            \
  }                                                                            \
  Style* set_##name(const type&(name)) {                                       \
    name##_ = name;                                                            \
    using_##name##_ = true;                                                    \
    modified_ = true;                                                          \
    return this;                                                               \
  };                                                                           \
  bool using_##name() const {                                                  \
    if (!using_##name##_ && base_style_) return base_style_->using_##name();   \
    return using_##name##_;                                                    \
  }

#define __AXGL_GUI_STYLE_APPLY_TO(name)                                        \
  if (using_##name##_) target.name##_ = name##_;                               \
  else if (using_##name()) target.name##_ = get_##name();

namespace axgl::gui {

enum class Display {
  kBlock,
  kInline,
  // kInlineBlock,
  // kFlex,
  // kGrid,
};

enum class Cursor {
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

class Style {
private:
  axgl::ptr_t<Style> base_style_;
  bool modified_ = false;

public:
  __AXGL_GUI_STYLE_PROPERTY(glm::vec4, color, {0.0f})
  __AXGL_GUI_STYLE_PROPERTY(float, opacity, = 1.0f)
  __AXGL_GUI_STYLE_PROPERTY(Cursor, cursor, = Cursor::kNormal)
  // content
  __AXGL_GUI_STYLE_PROPERTY(std::vector<std::string>, fonts, )
  __AXGL_GUI_STYLE_PROPERTY(glm::vec4, font_color, {1.0f})
  __AXGL_GUI_STYLE_PROPERTY(float, font_size, = 16.0f)
  // layout
  __AXGL_GUI_STYLE_PROPERTY(Display, display, = Display::kBlock)
  __AXGL_GUI_STYLE_PROPERTY(glm::vec4, margin, {0.0f})
  __AXGL_GUI_STYLE_PROPERTY(glm::vec4, padding, {0.0f})

  Style* set_base_style(axgl::ptr_t<Style> style) {
    for (auto* node = style.get(); node != nullptr;) {
      if (node == this) {
#ifdef AXGL_DEBUG
        AXGL_LOG_WARN("Ignored base style: circular reference detected");
#endif
        return this;
      }
      node = node->base_style_.get();
    }
    base_style_ = std::move(style);
    modified_ = true;
    return this;
  }

  [[nodiscard]] bool is_modified() const {
    if (!modified_ && base_style_) return base_style_->is_modified();
    return modified_;
  }
  void reset_modified() { modified_ = false; }

  void apply_to(Style& target) const {
    __AXGL_GUI_STYLE_APPLY_TO(color);
    __AXGL_GUI_STYLE_APPLY_TO(opacity);
    __AXGL_GUI_STYLE_APPLY_TO(cursor);
    // content
    __AXGL_GUI_STYLE_APPLY_TO(fonts);
    __AXGL_GUI_STYLE_APPLY_TO(font_color);
    __AXGL_GUI_STYLE_APPLY_TO(font_size);
    // layout
    __AXGL_GUI_STYLE_APPLY_TO(display);
    __AXGL_GUI_STYLE_APPLY_TO(margin);
    __AXGL_GUI_STYLE_APPLY_TO(padding);
  }
};

} // namespace axgl::gui
