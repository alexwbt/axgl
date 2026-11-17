#pragma once

#include <algorithm>
#include <functional>

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/toggle_element.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/gui/elements/text_element.hpp>

namespace axgl::impl::opengl::gui {

class ToggleElement : virtual public axgl::gui::ToggleElement,
                      public axgl::impl::opengl::gui::TextElement {
  ToggleElement::Handler handler_;
  bool checked_ = false;

public:
  ToggleElement() { set_focusable(true); }

  void set_checked(bool checked) override { checked_ = checked; }

  [[nodiscard]] bool is_checked() const override { return checked_; }

  void on_toggle(ToggleElement::Handler handler) override {
    handler_ = std::move(handler);
  }

  void on_activate(const axgl::gui::Context& context) override {
    axgl::impl::opengl::gui::TextElement::on_activate(context);
    checked_ = !checked_;
    if (handler_) handler_(context, checked_);
    context.page->set_should_render(true);
  }

  void on_keydown(
    const axgl::gui::Context& context, const axgl::KeyEvent& event
  ) override {
    using enum axgl::Input::Source;
    if (event.key == kKeySpace || event.key == kKeyEnter) {
      checked_ = !checked_;
      if (handler_) handler_(context, checked_);
      context.page->set_should_render(true);
    }
  }

  [[nodiscard]] glm::vec2 measure_content(
    const axgl::gui::Context& context, const glm::vec2& available_size
  ) override {
    const auto text_size
      = axgl::impl::opengl::gui::TextElement::measure_content(
        context, available_size
      );
    box_and_gap_ = text_size.y;
    return text_size + glm::vec2(box_and_gap_, 0.0f);
  }

  void render(const axgl::gui::Context& context) override {
    render_base(context);

    const auto padding = computed_style_->get_padding() * context.scale;
    const auto content_position = position_ + glm::vec2(padding.w, padding.x);
    const float box = box_and_gap_;
    const float gap = box * 0.5f;

    draw_quad(
      context,
      content_position,
      {box, box},
      box_color_,
      computed_style_->get_opacity()
    );

    if (checked_) {
      const float inset = box * 0.25f;
      draw_quad(
        context,
        content_position + glm::vec2(inset, inset),
        {box - inset * 2.0f, box - inset * 2.0f},
        computed_style_->get_font_color(),
        computed_style_->get_opacity()
      );
    }

    const bool hovered = is_hovering() || is_focused();
    if (hovered) {
      draw_quad(
        context,
        content_position,
        {box, box},
        {1.0f, 1.0f, 1.0f, 0.15f},
        computed_style_->get_opacity()
      );
    }

    draw_text_at(content_position + glm::vec2(box + gap, 0.0f), context);
  }

protected:
  glm::vec4 box_color_{1.0f, 1.0f, 1.0f, 0.3f};

private:
  float box_and_gap_ = 0.0f;

  void draw_text_at(
    const glm::vec2& position, const axgl::gui::Context& context
  ) {
    if (!text_texture_) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    const auto& opacity = computed_style_->get_opacity();
    const auto& font_color = computed_style_->get_font_color();
    const auto size = glm::vec3(
      text_texture_->get_width(), text_texture_->get_height(), 1.0f
    );
    const auto model
      = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
      * glm::scale(size);
    const auto& shader = Shaders::instance().gui();
    shader.use_program();
    text_texture_->use(GL_TEXTURE0);
    shader.set_int("background_texture", 0);
    shader.set_bool("use_texture", true);
    shader.set_vec4("color", font_color);
    shader.set_float("opacity", opacity);
    shader.set_mat4("projection_view_model", context.projection * model);
    ::opengl::StaticVAOs::instance().quad().draw();
  }
};

} // namespace axgl::impl::opengl::gui
