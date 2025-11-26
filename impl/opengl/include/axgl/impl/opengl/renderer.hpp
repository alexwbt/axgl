#pragma once

#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include <axgl/impl/glfw/window.hpp>

namespace axgl::impl
{

class OpenglRenderer : public axgl::Renderer
{
  int clear_bit_ = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
  GLfloat clear_color_r_ = 0.0f;
  GLfloat clear_color_g_ = 0.0f;
  GLfloat clear_color_b_ = 0.0f;
  GLfloat clear_color_a_ = 0.0f;

  bool initialized_glad_ = false;
  std::shared_ptr<GlfwWindow> window_;

  bool after_render_ = false;
  std::vector<std::pair<float, axgl::Component*>> blend_renders_;

public:
  void add_blend_render(float distance2, axgl::Component* component)
  {
    ZoneScopedN("Add Blend Render");
    blend_renders_.emplace_back(distance2, component);
  }

  [[nodiscard]] bool is_after_render() const { return after_render_; }

  bool ready() override { return window_ && window_->ready(); }

  void before_render() override
  {
    ZoneScopedN("Renderer Before Render");
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

    after_render_ = false;
  }

  void after_render() override
  {
    after_render_ = true;

    if (!blend_renders_.empty())
    {
      ZoneScopedN("Renderer Render Blending Components");

      // sort blend_renders_ by distance descending
      std::ranges::sort(blend_renders_, [](const auto& a, const auto& b)
      {
        return a.first > b.first;
      });
      // render blending components
      for (const auto& component : blend_renders_ | std::views::values)
        component->render();
      // clear list
      blend_renders_.clear();
    }

    ZoneScopedN("Renderer After Render");
    if (!window_)
      return;

    window_->swap_buffers();

    glDisable(GL_MULTISAMPLE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
  }

  void set_window(std::shared_ptr<axgl::Window> window) override
  {
    window_ = std::dynamic_pointer_cast<GlfwWindow>(std::move(window));
    if (!window_)
#ifdef AXGL_DEBUG
      throw std::runtime_error("The provided window is not a valid GlfwWindow instance. "
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

} // namespace axgl::impl

