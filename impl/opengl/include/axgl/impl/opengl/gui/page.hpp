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
  axgl::impl::gui::ElementContainer elements_;
  ::opengl::Framebuffer framebuffer_;

public:
  void set_size(uint32_t width, uint32_t height) override
  {
    width_ = width;
    height_ = height;
  }

  void render(const axgl::Service::Context& context, axgl::ptr_t<axgl::Texture> texture) override
  {
    const auto texture_impl = axgl::ptr_cast<axgl::impl::opengl::Texture>(texture);
#ifdef AXGL_DEBUG
    if (!texture_impl)
      throw std::runtime_error("axgl::impl::opengl::Texture is required to use axgl::impl::opengl::gui::Page");
#endif

    framebuffer_.attach_texture(GL_COLOR_ATTACHMENT0, *texture_impl->get_texture());
    framebuffer_.set_draw_buffers({GL_COLOR_ATTACHMENT0});
    framebuffer_.use();
    glViewport(0, 0, width_, height_);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const axgl::gui::Page::Context current_context{
      context.axgl, context.delta_tick, *context.axgl.gui_service(), *this, nullptr};
    for (const auto& child : elements_.get())
      child->render(current_context);
  }

  [[nodiscard]] axgl::Container<axgl::gui::Element>& elements() override { return elements_; }
};

} // namespace axgl::impl::opengl::gui
