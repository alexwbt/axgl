#pragma once

#include <algorithm>
#include <functional>

#include <utf8.h>

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/input_element.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/gui/elements/text_element.hpp>

namespace axgl::impl::opengl::gui {

class InputElement : virtual public axgl::gui::InputElement,
                     public axgl::impl::opengl::gui::TextElement {
  std::function<void(const axgl::gui::Context&)> submit_handler_;
  std::size_t cursor_ = 0;
  std::uint64_t caret_tick_ = 0;

public:
  InputElement() { set_focusable(true); }

  void on_submit(
    std::function<void(const axgl::gui::Context&)> handler
  ) override {
    submit_handler_ = std::move(handler);
  }

  void set_text(const std::string& text) override {
    axgl::impl::opengl::gui::TextElement::set_text(text);
    cursor_ = text_.size();
  }

  void update(const axgl::gui::Context& context) override {
    axgl::impl::opengl::gui::TextElement::update(context);

    if (!is_focused()) {
      caret_tick_ = 0;
      return;
    }

    handle_text_input(context);
    caret_tick_++;
  }

  void render(const axgl::gui::Context& context) override {
    axgl::impl::opengl::gui::TextElement::render(context);
    if (is_focused()) draw_caret(context);
  }

private:
  void handle_text_input(const axgl::gui::Context& context) {
    const auto input_service = context.axgl->input_service();
    if (!input_service) return;

    bool changed = false;
    for (const auto& event : input_service->get_text_input_events()) {
      switch (event.type) {
      case axgl::TextInputEvent::Type::kChar:
        if (event.codepoint >= 0x20 && event.codepoint != 0x7f) {
          insert_codepoint(event.codepoint);
          changed = true;
        }
        break;
      case axgl::TextInputEvent::Type::kBackspace:
        changed |= erase_previous();
        break;
      case axgl::TextInputEvent::Type::kDelete: changed |= erase_next(); break;
      case axgl::TextInputEvent::Type::kLeft: move_previous(); break;
      case axgl::TextInputEvent::Type::kRight: move_next(); break;
      case axgl::TextInputEvent::Type::kEnter:
        if (submit_handler_) submit_handler_(context);
        break;
      }
    }

    if (changed) {
      measured_ = false;
      context.page->set_should_render(true);
    }
  }

  void insert_codepoint(char32_t codepoint) {
    std::string encoded;
    utf8::append(codepoint, encoded);
    text_.insert(cursor_, encoded);
    cursor_ += encoded.size();
  }

  bool erase_previous() {
    if (cursor_ == 0) return false;
    const auto current = text_.begin() + static_cast<std::ptrdiff_t>(cursor_);
    auto previous = current;
    utf8::prior(previous, text_.begin());
    text_.erase(previous, current);
    cursor_ = static_cast<std::size_t>(previous - text_.begin());
    return true;
  }

  bool erase_next() {
    if (cursor_ >= text_.size()) return false;
    const auto current = text_.begin() + static_cast<std::ptrdiff_t>(cursor_);
    auto next = current;
    utf8::next(next, text_.end());
    text_.erase(current, next);
    return true;
  }

  void move_previous() {
    if (cursor_ == 0) return;
    const auto current = text_.begin() + static_cast<std::ptrdiff_t>(cursor_);
    auto previous = current;
    utf8::prior(previous, text_.begin());
    cursor_ = static_cast<std::size_t>(previous - text_.begin());
  }

  void move_next() {
    if (cursor_ >= text_.size()) return;
    const auto current = text_.begin() + static_cast<std::ptrdiff_t>(cursor_);
    auto next = current;
    utf8::next(next, text_.end());
    cursor_ = static_cast<std::size_t>(next - text_.begin());
  }

  void draw_caret(const axgl::gui::Context& context) {
    if ((caret_tick_ / 30) % 2 != 0) return;

    const auto padding = computed_style_->get_padding() * context.scale;
    const auto content_position = position_ + glm::vec2(padding.w, padding.x);

    const auto cursor = text_.begin() + static_cast<std::ptrdiff_t>(cursor_);
    const auto codepoint_index = utf8::distance(text_.begin(), cursor);
    const auto codepoint_count = utf8::distance(text_.begin(), text_.end());
    const float ratio = codepoint_count > 0
      ? static_cast<float>(codepoint_index)
        / static_cast<float>(codepoint_count)
      : 0.0f;

    const float text_width = text_texture_
      ? util::clamp_cast<float>(text_texture_->get_width())
      : 0.0f;
    const float caret_height
      = computed_style_->get_font_size() * context.scale * context.font_scale;

    draw_quad(
      context,
      content_position + glm::vec2(text_width * ratio, 0.0f),
      {std::max(1.0f, context.scale), caret_height},
      computed_style_->get_font_color(),
      computed_style_->get_opacity()
    );
  }
};

} // namespace axgl::impl::opengl::gui
