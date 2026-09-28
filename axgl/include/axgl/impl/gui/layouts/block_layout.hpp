#pragma once

#include <algorithm>

#include <axgl/interface/gui/context.hpp>
#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/gui/layout.hpp>

namespace axgl::impl::gui {

class BlockLayout : public axgl::gui::Layout {
public:
  [[nodiscard]] glm::vec2 measure(
    const axgl::gui::Context& context,
    axgl::Container<axgl::gui::Element>& elements,
    const glm::vec2& available_size
  ) const override {
    using namespace axgl::gui;

    float y = 0.0f;
    float line_width = 0.0f;
    float line_height = 0.0f;
    float max_width = 0.0f;

    for (const auto& element : elements.get()) {
      const auto& style = element->get_computed_style();
      const auto margin = style.get_margin() * context.scale;
      const float margin_top = margin.x;
      const float margin_right = margin.y;
      const float margin_bottom = margin.z;
      const float margin_left = margin.w;

      switch (style.get_display()) {
      case Display::kBlock: {
        const float width
          = std::max(0.0f, available_size.x - margin_left - margin_right);
        element->measure(context, {width, available_size.y});
        const auto desired = element->get_desired_size();
        y += margin_top + desired.y + margin_bottom;
        max_width = std::max(max_width, width);
        line_width = 0.0f;
        line_height = 0.0f;
        break;
      }
      case Display::kInline: {
        const float width
          = std::max(0.0f, available_size.x - margin_left - margin_right);
        element->measure(context, {width, available_size.y});
        const auto desired = element->get_desired_size();
        const float outer_width = margin_left + desired.x + margin_right;
        if (line_width + outer_width > available_size.x && line_width > 0.0f) {
          y += line_height;
          max_width = std::max(max_width, line_width);
          line_width = 0.0f;
          line_height = 0.0f;
        }
        line_width += outer_width;
        line_height
          = std::max(line_height, margin_top + desired.y + margin_bottom);
        break;
      }
      }
    }

    y += line_height;
    max_width = std::max(max_width, line_width);
    return {max_width, y};
  }

  void arrange(
    const axgl::gui::Context& context,
    axgl::Container<axgl::gui::Element>& elements,
    const glm::vec2& content_origin,
    const glm::vec2& available_size
  ) const override {
    using namespace axgl::gui;

    const float origin_x = content_origin.x;
    const float right_edge = content_origin.x + available_size.x;
    float x = origin_x;
    float y = content_origin.y;
    float line_height = 0.0f;

    for (const auto& element : elements.get()) {
      const auto& style = element->get_computed_style();
      const auto margin = style.get_margin() * context.scale;
      const float margin_top = margin.x;
      const float margin_right = margin.y;
      const float margin_bottom = margin.z;
      const float margin_left = margin.w;
      auto size = element->get_desired_size();

      switch (style.get_display()) {
      case Display::kBlock:
        y += margin_top;
        x = origin_x + margin_left;
        size.x = std::max(0.0f, available_size.x - margin_left - margin_right);
        element->arrange(context, {x, y}, size);
        x = origin_x;
        y += size.y + margin_bottom;
        break;
      case Display::kInline: {
        const float outer_width = margin_left + size.x + margin_right;
        if (x + outer_width > right_edge && x > origin_x) {
          x = origin_x;
          y += line_height;
          line_height = 0.0f;
        }
        x += margin_left;
        element->arrange(context, {x, y}, size);
        x += size.x + margin_right;
        line_height
          = std::max(line_height, margin_top + size.y + margin_bottom);
        break;
      }
      }
    }
  }
};

} // namespace axgl::impl::gui
