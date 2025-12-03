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
  axgl::ptr_t<glfw::Window> window_;

  std::vector<const axgl::Light*> lights_;

public:
  void render(const axgl::Service::Context& context, axgl::ptr_t<axgl::Realm> realm) override
  {
    if (!window_ || !window_->ready())
    {
#ifdef AXGL_DEBUG
      SPDLOG_WARN("Unable to render realm: window is not set or not ready.");
#endif
      return;
    }

    auto* camera = context.axgl.camera_service()->get_camera();
    if (!camera)
    {
#ifdef AXGL_DEBUG
      SPDLOG_WARN("Unable to render realm: camera is not set.");
#endif
      return;
    }

    axgl::Renderer::Context render_context{context, *this, *realm, *camera};

    const auto viewport = window_->get_size();
    if (const auto v = glm::vec2(viewport); camera->viewport != v)
    {
      camera->viewport.x = v.x;
      camera->viewport.y = v.y;
      camera->update_projection_view_matrix();
    }

    glViewport(0, 0, viewport.x, viewport.y);
    glClearColor(clear_color_r_, clear_color_g_, clear_color_b_, clear_color_a_);
    glClear(clear_bit_);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render realm
    for (const auto& entity : realm->entities().get())
      for (const auto& component : entity->components().get())
        if (const auto renderable = axgl::ptr_cast<axgl::Renderable>(component))
          renderable->render(render_context, *entity);

    window_->swap_buffers();

    glDisable(GL_MULTISAMPLE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
  }

  void set_window(axgl::ptr_t<axgl::Window> window) override
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
};

} // namespace axgl::impl::opengl
