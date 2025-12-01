#pragma once

#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <axgl/interface/renderer.hpp>

#include <axgl/axgl.hpp>
#include <axgl/impl/glfw/window.hpp>

namespace axgl::impl::opengl
{

class Renderer : public axgl::Renderer
{
  int clear_bit_ = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
  GLfloat clear_color_r_ = 0.0f;
  GLfloat clear_color_g_ = 0.0f;
  GLfloat clear_color_b_ = 0.0f;
  GLfloat clear_color_a_ = 0.0f;

  bool initialized_glad_ = false;
  std::shared_ptr<glfw::Window> window_;

  std::vector<const Renderable*> default_renderables_;
  std::vector<const Renderable*> blend_renderables_;

public:
  bool ready() override { return window_ && window_->ready(); }

  void render() override
  {
    if (!window_)
      return;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    const auto size = window_->get_size();
    glViewport(0, 0, size.x, size.y);

    glClearColor(clear_color_r_, clear_color_g_, clear_color_b_, clear_color_a_);
    glClear(clear_bit_);

    // render
    {
      ZoneScopedN("Renderer Render");

      for (const auto* renderable : default_renderables_)
        renderable->render();

      // render blended
      std::ranges::sort(
        blend_renderables_, [](const auto* a, const auto* b)
      {
        return a->sorted > b->sorted;
      });
      for (const auto* renderable : blend_renderables_)
        renderable->render();
    }

    window_->swap_buffers();

    glDisable(GL_MULTISAMPLE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
  }

  void add_renderable(const Renderable* renderable) override
  {
    switch (renderable->stage)
    {
    case Stage::kBlend: blend_renderables_.push_back(renderable); break;
    default: default_renderables_.push_back(renderable); break;
    }
  }

  void remove_renderable(const Renderable* renderable) override
  {
    switch (renderable->stage)
    {
    case Stage::kBlend: blend_renderables_.erase(std::ranges::find(blend_renderables_, renderable)); break;
    default: default_renderables_.erase(std::ranges::find(default_renderables_, renderable)); break;
    }
  }

  void set_window(std::shared_ptr<axgl::Window> window) override
  {
    window_ = std::dynamic_pointer_cast<glfw::Window>(std::move(window));
    if (!window_)
#ifdef AXGL_DEBUG
      throw std::runtime_error(
        "The provided window is not a valid GlfwWindow instance. "
        "GlfwWindow is required for OpenglRenderer.");
#else
      return;
#endif

    window_->use();

    // initialize glad
    if (!initialized_glad_ && !gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
      SPDLOG_CRITICAL("Failed to initialize GLAD.");
    initialized_glad_ = true;
  }

  [[nodiscard]] glm::ivec2 viewport() const override { return window_->get_size(); }
};

} // namespace axgl::impl::opengl

