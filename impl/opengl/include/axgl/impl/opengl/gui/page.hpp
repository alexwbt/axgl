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

  [[nodiscard]] glm::ivec2 get_size() const override { return {width_, height_}; }

  [[nodiscard]] bool should_render() const override { return elements_.should_render(); }

  void render(const axgl::Service::Context& context, axgl::ptr_t<axgl::Texture> texture) override
  {
    const auto texture_impl = axgl::ptr_cast<axgl::impl::opengl::Texture>(texture);
#ifdef AXGL_DEBUG
    if (!texture_impl)
      throw std::runtime_error("axgl::impl::opengl::Texture is required to use axgl::impl::opengl::gui::Page");
#endif

    const auto texture_ptr = texture_impl->get_texture();
    if (!texture_ptr->loaded())
    {
      texture_ptr->load_texture(0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
      texture_ptr->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      texture_ptr->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      texture_ptr->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      texture_ptr->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    framebuffer_.attach_texture(GL_COLOR_ATTACHMENT0, *texture_ptr);
    framebuffer_.set_draw_buffers({GL_COLOR_ATTACHMENT0});
    framebuffer_.use();
    glViewport(0, 0, width_, height_);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 projection = glm::ortho(static_cast<float>(width_), 0.0f, static_cast<float>(height_), 0.0f);
    const axgl::gui::Page::Context current_context{context, *context.axgl.gui_service(), *this, projection, nullptr};
    for (const auto& child : elements_.get())
      child->render(current_context);
  }

  [[nodiscard]] axgl::Container<axgl::gui::Element>& elements() override { return elements_; }
};

} // namespace axgl::impl::opengl::gui
