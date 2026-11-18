#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/dropdown_element.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/gui/elements/text_element.hpp>

namespace axgl::impl::opengl::gui {

class DropdownElement : virtual public axgl::gui::DropdownElement,
                        public axgl::impl::opengl::gui::TextElement {
  DropdownElement::Handler handler_;
  std::vector<std::string> options_;
  std::size_t selected_ = 0;
  bool open_ = false;

public:
  DropdownElement() { set_focusable(true); }

  void set_options(const std::vector<std::string>& options) override {
    options_ = options;
    if (selected_ >= options_.size())
      selected_ = options_.empty() ? 0 : options_.size() - 1;
    set_selected_label();
  }

  [[nodiscard]] const std::vector<std::string>& get_options() const override {
    return options_;
  }

  void set_selected(std::size_t index) override {
    if (index < options_.size()) {
      selected_ = index;
      set_selected_label();
    }
  }

  [[nodiscard]] std::size_t get_selected() const override { return selected_; }

  void on_change(DropdownElement::Handler handler) override {
    handler_ = std::move(handler);
  }

  void on_activate(const axgl::gui::Context& context) override {
    axgl::impl::opengl::gui::TextElement::on_activate(context);
    if (!options_.empty()) {
      open_ = !open_;
      context.page->set_should_render(true);
    }
  }

  void on_blur(const axgl::gui::Context& context) override {
    axgl::impl::opengl::gui::TextElement::on_blur(context);
    open_ = false;
    context.page->set_should_render(true);
  }

  void on_keydown(
    const axgl::gui::Context& context, const axgl::KeyEvent& event
  ) override {
    using enum axgl::InputSource;
    if (event.key == kKeyUp && !options_.empty()) {
      set_selected((selected_ + options_.size() - 1) % options_.size());
      emit_change(context);
    } else if (event.key == kKeyDown && !options_.empty()) {
      set_selected((selected_ + 1) % options_.size());
      emit_change(context);
    } else if (event.key == kKeyEnter || event.key == kKeySpace) {
      if (!options_.empty()) {
        open_ = !open_;
        context.page->set_should_render(true);
      }
    }
  }

  void on_pointer_enter(const axgl::gui::Context& context) override {
    axgl::impl::opengl::gui::TextElement::on_pointer_enter(context);
    context.page->set_cursor(axgl::gui::Cursor::kPointer);
  }

  void render(const axgl::gui::Context& context) override {
    axgl::impl::opengl::gui::TextElement::render(context);

    // arrow indicator
    const auto padding = computed_style_->get_padding() * context.scale;
    const auto content_position = position_ + glm::vec2(padding.w, padding.x);
    const float arrow_size
      = computed_style_->get_font_size() * context.scale * 0.5f;
    const glm::vec2 arrow_center{
      content_position.x + size_.x - padding.w - arrow_size,
      content_position.y + (size_.y - padding.x - padding.z) * 0.5f
    };
    draw_quad(
      context,
      {arrow_center.x - arrow_size * 0.3f, arrow_center.y - arrow_size * 0.3f},
      {arrow_size * 0.6f, arrow_size * 0.6f},
      computed_style_->get_font_color(),
      computed_style_->get_opacity()
    );

    if (!open_) return;

    // open list of options
    const float row_height = computed_style_->get_font_size() * context.scale
      + padding.x + padding.z;
    glm::vec2 pos{
      content_position.x, content_position.y + (size_.y - padding.x - padding.z)
    };
    for (std::size_t i = 0; i < options_.size(); ++i) {
      const auto& option = options_[i];
      if (i == selected_) {
        draw_quad(
          context,
          pos,
          {size_.x - padding.y - padding.w, row_height},
          {1.0f, 1.0f, 1.0f, 0.2f},
          1.0f
        );
      } else {
        draw_quad(
          context,
          pos,
          {size_.x - padding.y - padding.w, row_height},
          {0.0f, 0.0f, 0.0f, 0.5f},
          1.0f
        );
      }
      draw_text_option(option, pos + glm::vec2(padding.w, 0.0f), context);
      pos.y += row_height;
    }
  }

private:
  void emit_change(const axgl::gui::Context& context) {
    if (handler_) handler_(context, selected_);
    context.page->set_should_render(true);
  }

  void set_selected_label() {
    if (selected_ < options_.size())
      axgl::impl::opengl::gui::TextElement::set_text(options_[selected_]);
  }

  void draw_text_option(
    const std::string& option,
    const glm::vec2& position,
    const axgl::gui::Context& context
  ) {
    if (option.empty()) return;
    const auto& text_service = context.axgl->text_service();
    const auto fonts = computed_style_->get_fonts();
    const auto font_size
      = computed_style_->get_font_size() * context.scale * context.font_scale;
    const auto texture = axgl::ptr_cast<axgl::impl::opengl::Texture>(
      text_service->create_texture({
        .value = option,
        .fonts = fonts,
        .font_color = computed_style_->get_font_color(),
        .font_size = font_size,
        .max_width = 1024,
        .vertical = false,
      })
    );
    if (!texture) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    const auto size
      = glm::vec3(texture->get_width(), texture->get_height(), 1.0f);
    const auto model
      = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
      * glm::scale(size);
    const auto& shader = Shaders::instance().gui();
    shader.use_program();
    texture->use(GL_TEXTURE0);
    shader.set_int("background_texture", 0);
    shader.set_bool("use_texture", true);
    shader.set_vec4("color", computed_style_->get_font_color());
    shader.set_float("opacity", computed_style_->get_opacity());
    shader.set_mat4("projection_view_model", context.projection * model);
    ::opengl::StaticVAOs::instance().quad().draw();
  }
};

} // namespace axgl::impl::opengl::gui
