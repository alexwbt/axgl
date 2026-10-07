#pragma once

#include <algorithm>

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/gui/element.hpp>
#include <axgl/impl/opengl/texture.hpp>

namespace axgl::impl::opengl::gui {

class TextElement : virtual public axgl::gui::TextElement,
                    public axgl::impl::opengl::gui::Element {
protected:
  std::string text_;
  axgl::ptr_t<axgl::impl::opengl::Texture> text_texture_;

  bool measured_ = false;
  std::string cached_text_;
  std::vector<std::string> cached_fonts_;
  glm::vec4 cached_font_color_{0.0f};
  float cached_font_size_ = 0.0f;
  float cached_max_width_ = 0.0f;

public:
  void set_text(const std::string& text) override {
    text_ = text;
    measured_ = false;
  }
  [[nodiscard]] std::string get_text() const override { return text_; }

  void render(const axgl::gui::Context& context) override {
    render_base(context);

    if (text_texture_) {
      // text texture is premultiplied (see opengl::TextRenderer::render_text)
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

      const auto& opacity = computed_style_->get_opacity();
      const auto& font_color = computed_style_->get_font_color();
      const auto padding = computed_style_->get_padding() * context.scale;
      const auto content_position = position_ + glm::vec2(padding.w, padding.x);
      const auto size = glm::vec3(
        text_texture_->get_width(), text_texture_->get_height(), 1.0f
      );
      const auto content_model
        = glm::translate(glm::mat4(1.0f), glm::vec3(content_position, 0.0f))
        * glm::scale(size);
      const auto& content_shader = Shaders::instance().gui();
      content_shader.use_program();
      text_texture_->use(GL_TEXTURE0);
      content_shader.set_int("background_texture", 0);
      content_shader.set_bool("use_texture", true);
      content_shader.set_vec4("color", font_color);
      content_shader.set_float("opacity", opacity);
      content_shader.set_mat4(
        "projection_view_model", context.projection * content_model
      );
      ::opengl::StaticVAOs::instance().quad().draw();
    }
  }

protected:
  [[nodiscard]] glm::vec2 measure_content(
    const axgl::gui::Context& context, const glm::vec2& available_size
  ) override {
    const auto padding = computed_style_->get_padding() * context.scale;
    const glm::vec2 padding_size{padding.y + padding.w, padding.x + padding.z};
    const glm::vec2 content_available{
      std::max(0.0f, available_size.x - padding_size.x),
      std::max(0.0f, available_size.y - padding_size.y)
    };

    const auto text_scale = context.scale * context.font_scale;
    const auto fonts = computed_style_->get_fonts();
    const auto font_color = computed_style_->get_font_color();
    const auto font_size = computed_style_->get_font_size() * text_scale;

    const bool dirty = !measured_ || text_ != cached_text_
      || fonts != cached_fonts_ || font_color != cached_font_color_
      || font_size != cached_font_size_
      || content_available.x != cached_max_width_;

    if (dirty) {
      measured_ = true;
      cached_text_ = text_;
      cached_fonts_ = fonts;
      cached_font_color_ = font_color;
      cached_font_size_ = font_size;
      cached_max_width_ = content_available.x;

      render_text_texture(
        context.axgl->text_service(), content_available.x, font_size
      );
      context.page->set_should_render(true);
    }

    glm::vec2 content{0.0f};
    if (text_texture_)
      content = {
        util::clamp_cast<float>(text_texture_->get_width()),
        util::clamp_cast<float>(text_texture_->get_height())
      };
    return content + padding_size;
  }

private:
  void render_text_texture(
    const axgl::ptr_t<axgl::TextService>& text_service,
    float max_width,
    float font_size
  ) {
    const auto fonts = computed_style_->get_fonts();
    const auto font_color = computed_style_->get_font_color();
    if (!text_.empty() && !fonts.empty() && font_size > 0.01f) {
      text_texture_ = axgl::ptr_cast<axgl::impl::opengl::Texture>(
        text_service->create_texture({
          .value = text_,
          .fonts = fonts,
          .font_color = font_color,
          .font_size = font_size,
          .max_width = util::clamp_cast<std::int32_t>(max_width),
          .vertical = false,
        })
      );
#ifdef AXGL_DEBUG
      if (!text_texture_)
        AXGL_LOG_WARN(
          "axgl::impl::opengl::Texture is required to use "
          "axgl::impl::opengl::gui::Element"
        );
#endif
    } else {
      text_texture_ = nullptr;
    }
  }
};

} // namespace axgl::impl::opengl::gui
