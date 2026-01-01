#pragma once

#include <glm/gtx/transform.hpp>

#include <axgl/interface/gui/element.hpp>
#include <axgl/interface/gui/page.hpp>

#include <axgl/impl/gui/element_base.hpp>

#include <opengl/static_shaders.hpp>
#include <opengl/static_vaos.hpp>

namespace axgl::impl::opengl::gui
{

class Element : virtual public axgl::gui::Element, public axgl::impl::gui::ElementBase
{
public:
  void on_pointer_enter(const axgl::gui::Page::Context& context) override
  {
    axgl::impl::gui::ElementBase::on_pointer_enter(context);
  }

  void update(const axgl::gui::Page::Context& context) override
  {
    ElementBase::update(context);

    if (should_render_content_)
    {
      const auto& style = current_style();
      const auto& text_service = context.axgl.text_service();
      content_texture_ = axgl::ptr_cast<axgl::impl::opengl::Texture>(text_service->create_texture({
        .value = content_,
        .fonts = style->get_fonts(),
        .font_color = style->get_font_color(),
        .font_size = style->get_font_size(),
        .vertical = false,
      }));
#ifdef AXGL_DEBUG
      if (!content_texture_)
        AXGL_LOG_WARN("axgl::impl::opengl::Texture is required to use axgl::impl::opengl::gui::Element");
#endif
      should_render_content_ = false;
    }
  }

  void render(const axgl::gui::Page::RenderContext& context) override
  {
    const auto& style = current_style();

    const auto model                                                //
      = glm::translate(glm::mat4(1.0f), glm::vec3(position_, 0.0f)) //
      * glm::scale(glm::vec3(size_.x, size_.y, 1.0f));              //
    auto& shader = ::opengl::StaticShaders::instance().gui();
    shader.use_program();
    shader.set_bool("use_texture", false);
    shader.set_vec4("color", style->get_color());
    shader.set_float("opacity", style->get_opacity());
    shader.set_mat4("projection_view_model", context.projection * model);
    ::opengl::StaticVAOs::instance().quad().draw();

    if (content_texture_)
    {
      const auto size = glm::vec3(content_texture_->get_width(), content_texture_->get_height(), 1.0f) * context.scale;
      const auto content_model                                        //
        = glm::translate(glm::mat4(1.0f), glm::vec3(position_, 0.0f)) //
        * glm::scale(size);                                           //
      auto& content_shader = ::opengl::StaticShaders::instance().gui();
      content_shader.use_program();
      content_texture_->use(GL_TEXTURE0);
      content_shader.set_int("background_texture", 0);
      content_shader.set_bool("use_texture", true);
      content_shader.set_vec4("color", style->get_font_color());
      content_shader.set_float("opacity", style->get_opacity());
      content_shader.set_mat4("projection_view_model", context.projection * content_model);
      ::opengl::StaticVAOs::instance().quad().draw();
    }

    glScissor(
      static_cast<GLint>(scissor_rect_.x),                             //
      context.page.get_size().y - static_cast<GLint>(scissor_rect_.w), //
      static_cast<GLsizei>(scissor_rect_.z - scissor_rect_.x),         //
      static_cast<GLsizei>(scissor_rect_.w - scissor_rect_.y));        //

    render_children(context);
  }
};

} // namespace axgl::impl::opengl::gui
