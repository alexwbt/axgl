#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/gui/elements/text_element.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/gui/element.hpp>
#include <axgl/impl/opengl/texture.hpp>

namespace axgl::impl::opengl::gui
{

class TextElement : virtual public axgl::gui::TextElement,
                    public axgl::impl::opengl::gui::Element
{
  std::string text_;
  float text_scale_ = 1.0f;
  bool modified_text_ = false;
  axgl::ptr_t<axgl::impl::opengl::Texture> text_texture_;

public:
  void set_text(const std::string& text) override
  {
    text_ = text;
    modified_text_ = true;
  }
  [[nodiscard]] std::string get_text() const override { return text_; }

  void update(const axgl::gui::Context& context) override
  {
    axgl::impl::opengl::gui::Element::update(context);

    if (modified_text_ || text_scale_ != context.scale)
    {
      modified_text_ = false;
      text_scale_ = context.scale;

      if (!text_.empty())
      {
        const auto& text_service = context.axgl->text_service();
        text_texture_ = axgl::ptr_cast<axgl::impl::opengl::Texture>(
          text_service->create_texture({
            .value = text_,
            .fonts = computed_style_->get_fonts(),
            .font_color = computed_style_->get_font_color(),
            .font_size = computed_style_->get_font_size() * text_scale_,
            .vertical = false,
          }));
#ifdef AXGL_DEBUG
        if (!text_texture_)
          AXGL_LOG_WARN(
            "axgl::impl::opengl::Texture is required to use "
            "axgl::impl::opengl::gui::Element");
#endif
      }
      else text_texture_ = nullptr;
    }
  }

  void render(const axgl::gui::Context& context) override
  {
    render_base(context);

    if (text_texture_)
    {
      const auto& opacity = computed_style_->get_opacity();
      const auto& font_color = computed_style_->get_font_color();
      const auto size = glm::vec3(
        text_texture_->get_width(), text_texture_->get_height(), 1.0f);
      const auto content_model                                        //
        = glm::translate(glm::mat4(1.0f), glm::vec3(position_, 0.0f)) //
        * glm::scale(size);                                           //
      auto& content_shader = ::opengl::StaticShaders::instance().gui();
      content_shader.use_program();
      text_texture_->use(GL_TEXTURE0);
      content_shader.set_int("background_texture", 0);
      content_shader.set_bool("use_texture", true);
      content_shader.set_vec4("color", font_color);
      content_shader.set_float("opacity", opacity);
      content_shader.set_mat4(
        "projection_view_model", context.projection * content_model);
      ::opengl::StaticVAOs::instance().quad().draw();
    }
  }
};

} // namespace axgl::impl::opengl::gui
