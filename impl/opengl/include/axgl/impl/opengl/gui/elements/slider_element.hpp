#pragma once

#include <algorithm>
#include <functional>

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/slider_element.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/gui/element.hpp>

namespace axgl::impl::opengl::gui {

class SliderElement : virtual public axgl::gui::SliderElement,
                      public axgl::impl::opengl::gui::Element {
  SliderElement::Handler handler_;
  float value_ = 0.0f;
  float min_ = 0.0f;
  float max_ = 1.0f;
  bool dragging_ = false;

public:
  SliderElement() { set_focusable(true); }

  void set_value(float value) override {
    value_ = std::clamp(value, min_, max_);
  }

  [[nodiscard]] float get_value() const override { return value_; }

  void set_range(float min, float max) override {
    min_ = min;
    max_ = max;
    value_ = std::clamp(value_, min_, max_);
  }

  void on_change(SliderElement::Handler handler) override {
    handler_ = std::move(handler);
  }

  void on_activate(const axgl::gui::Context& context) override {
    axgl::impl::opengl::gui::Element::on_activate(context);
    dragging_ = true;
    context.page->set_should_render(true);
  }

  void on_deactivate(const axgl::gui::Context& context) override {
    axgl::impl::opengl::gui::Element::on_deactivate(context);
    dragging_ = false;
    context.page->set_should_render(true);
  }

  void on_pointer_exit(const axgl::gui::Context& context) override {
    axgl::impl::opengl::gui::Element::on_pointer_exit(context);
    if (dragging_) {
      dragging_ = false;
      context.page->set_should_render(true);
    }
  }

  void on_keydown(
    const axgl::gui::Context& context, const axgl::KeyEvent& event
  ) override {
    using enum axgl::InputSource;
    const float step = (max_ - min_) * 0.05f;
    if (event.key == kKeyLeft || event.key == kKeyDown) {
      set_value(value_ - step);
      if (handler_) handler_(context, value_);
    } else if (event.key == kKeyRight || event.key == kKeyUp) {
      set_value(value_ + step);
      if (handler_) handler_(context, value_);
    }
  }

  void update(const axgl::gui::Context& context) override {
    axgl::impl::opengl::gui::Element::update(context);

    float cursor_x = -1.0f;
    if (const auto& pointer = context.page->get_cursor_pointer()) {
      cursor_x = pointer->position.x;
    }

    if (dragging_ && cursor_x >= 0.0f) {
      const float padding_x = computed_style_->get_padding().w * context.scale;
      const float left = position_.x + padding_x;
      const float track_width = std::max(1.0f, size_.x - padding_x * 2.0f);
      const float ratio
        = std::clamp((cursor_x - left) / track_width, 0.0f, 1.0f);
      const float old_value = value_;
      value_ = min_ + (max_ - min_) * ratio;
      if (value_ != old_value) {
        if (handler_) handler_(context, value_);
        context.page->set_should_render(true);
      }
    }
  }

  void render(const axgl::gui::Context& context) override {
    render_base(context);

    const auto padding = computed_style_->get_padding() * context.scale;
    const auto content_position = position_ + glm::vec2(padding.w, padding.x);
    const float track_width = std::max(1.0f, size_.x - padding.w - padding.y);
    const float track_height = std::max(1.0f, context.scale * 4.0f);
    const float track_y = content_position.y
      + (size_.y - padding.x - padding.z) * 0.5f - track_height * 0.5f;

    draw_quad(
      context,
      {content_position.x, track_y},
      {track_width, track_height},
      {1.0f, 1.0f, 1.0f, 0.3f},
      computed_style_->get_opacity()
    );

    const float ratio = (max_ > min_) ? (value_ - min_) / (max_ - min_) : 0.0f;
    const float thumb_radius = std::max(8.0f * context.scale, track_height);
    const float thumb_x = content_position.x + track_width * ratio;
    const float thumb_y = track_y + track_height * 0.5f;

    draw_quad(
      context,
      {thumb_x - thumb_radius * 0.5f, thumb_y - thumb_radius * 0.5f},
      {thumb_radius, thumb_radius},
      computed_style_->get_font_color(),
      computed_style_->get_opacity()
    );
  }
};

} // namespace axgl::impl::opengl::gui
