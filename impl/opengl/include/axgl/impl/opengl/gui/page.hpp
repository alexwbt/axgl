#pragma once

#include "axgl/axgl.hpp"

#include <axgl/common.hpp>
#include <axgl/interface/gui/page.hpp>

#include <axgl/impl/gui/element_container.hpp>
#include <axgl/impl/opengl/texture.hpp>

#include <opengl/framebuffer.hpp>

namespace axgl::impl::opengl::gui
{

class Page : virtual public axgl::gui::Page
{
  uint32_t width_ = 0;
  uint32_t height_ = 0;
  std::unique_ptr<::opengl::Framebuffer> framebuffer_;
  axgl::ptr_t<axgl::impl::opengl::Texture> texture_;
  axgl::impl::gui::ElementContainer elements_;

public:
  void set_size(uint32_t width, uint32_t height) override
  {
    width_ = width;
    height_ = height;
  }

  void init(const axgl::Service::Context& context) override
  {
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
    framebuffer_->set_draw_buffers({GL_COLOR_ATTACHMENT0});
  }

  void render(const axgl::Service::Context& context) override
  {
    framebuffer_->use();
    glViewport(0, 0, static_cast<GLsizei>(width_), static_cast<GLsizei>(height_));
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_CULL_FACE);

    const glm::mat4 projection = glm::ortho(static_cast<float>(width_), 0.0f, static_cast<float>(height_), 0.0f);
    const axgl::gui::Page::Context current_context{context, *context.axgl.gui_service(), *this, projection, nullptr};
    for (const auto& child : elements_.get())
      child->render(current_context);
  }

  [[nodiscard]] glm::ivec2 get_size() const override { return {width_, height_}; }

  [[nodiscard]] bool should_render() const override { return elements_.should_render(); }

  [[nodiscard]] axgl::ptr_t<axgl::Texture> get_texture() override { return texture_; }

  [[nodiscard]] axgl::Container<axgl::gui::Element>& elements() override { return elements_; }
};

} // namespace axgl::impl::opengl::gui
