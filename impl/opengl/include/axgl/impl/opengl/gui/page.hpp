#pragma once

#include <axgl/axgl.hpp>

#include <axgl/common.hpp>
#include <axgl/interface/gui/page.hpp>

#include <axgl/impl/gui/page_base.hpp>
#include <axgl/impl/opengl/texture.hpp>

#include <opengl/framebuffer.hpp>

namespace axgl::impl::opengl::gui
{

class Page : virtual public axgl::gui::Page, public axgl::impl::gui::PageBase
{
  std::unique_ptr<::opengl::Framebuffer> framebuffer_;
  axgl::ptr_t<axgl::impl::opengl::Texture> texture_;

public:
  void init(const axgl::Service::Context& context) override
  {
    axgl::impl::gui::PageBase::init(context);

    const auto texture = context.axgl.renderer_service()->create_texture();
    texture_ = axgl::ptr_cast<axgl::impl::opengl::Texture>(texture);
#ifdef AXGL_DEBUG
    if (!texture_)
      throw std::runtime_error("axgl::impl::opengl::Texture is required to use axgl::impl::opengl::gui::Page");
#endif

    const auto opengl_texture = texture_->get_texture();
    opengl_texture->load_texture(
      0, GL_RGBA, static_cast<GLsizei>(width_), static_cast<GLsizei>(height_), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    opengl_texture->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    opengl_texture->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    opengl_texture->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    opengl_texture->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    framebuffer_ = std::make_unique<::opengl::Framebuffer>();
    framebuffer_->attach_texture(GL_COLOR_ATTACHMENT0, *opengl_texture);
    framebuffer_->set_draw_buffers({GL_COLOR_ATTACHMENT0, GL_STENCIL_ATTACHMENT});
  }

  void render(const axgl::Service::Context& context) override
  {
    axgl::impl::gui::PageBase::render(context);

    framebuffer_->use();
    const auto width = static_cast<GLsizei>(width_);
    const auto height = static_cast<GLsizei>(height_);
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, width, height);

    const glm::mat4 projection = glm::ortho(static_cast<float>(width_), 0.0f, static_cast<float>(height_), 0.0f);
    const axgl::gui::Page::RenderContext current_context{
      context, *context.axgl.gui_service(), *this, nullptr, nullptr, scale_, projection};
    for (const auto& child : elements_.get())
      child->render(current_context);

    glDisable(GL_SCISSOR_TEST);
  }

  [[nodiscard]] axgl::ptr_t<axgl::Texture> get_texture() const override { return texture_; }
};

} // namespace axgl::impl::opengl::gui
