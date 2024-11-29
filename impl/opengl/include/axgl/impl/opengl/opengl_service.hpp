#pragma once

#include <axgl/axgl.hpp>
#include <axgl/namespace.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/impl/glfw/glfw_service.hpp>

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

NAMESPACE_AXGL_IMPL

class OpenglRenderer : public interface::Renderer
{
private:
  int clear_bit_ = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
  GLfloat clear_color_r_ = 0.0f;
  GLfloat clear_color_g_ = 0.0f;
  GLfloat clear_color_b_ = 0.0f;
  GLfloat clear_color_a_ = 0.0f;

  bool initialized_glad_ = false;
  std::shared_ptr<GlfwWindow> window_;
  GLsizei window_width_ = 0;
  GLsizei window_height_ = 0;

public:
  bool ready() override
  {
    return window_ && window_->ready();
  }

  void before_render() override
  {
    if (!window_) return;
    window_->use();

    const auto& size = window_->get_size();
    if (size.width != window_width_ || size.height != window_height_)
    {
      window_width_ = size.width;
      window_height_ = size.height;
      glViewport(0, 0, window_width_, window_height_);
    }

    glClearColor(clear_color_r_, clear_color_g_, clear_color_b_, clear_color_a_);
    glClear(clear_bit_);
  }

  void after_render() override
  {
    if (!window_) return;
    window_->swap_buffers();
  }

  void set_window(std::shared_ptr<interface::Window> window) override
  {
    window_ = dynamic_pointer_cast<GlfwWindow>(std::move(window));
    if (!window_)
      throw std::runtime_error(
        "The provided window is not a valid GlfwWindow instance. "
        "GlfwWindow is required for OpenglRenderer.");

    window_->use();

    // initialize glad
    if (!initialized_glad_ && !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      SPDLOG_CRITICAL("Failed to initialize GLAD.");
    initialized_glad_ = true;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  glm::ivec2 viewport() const override
  {
    return { window_width_, window_height_ };
  }
};

class OpenglRendererService : public interface::RendererService
{
public:
  void initialize() override {}
  void terminate() override {}
  void update() override {}
  void render() override {}
  bool running() override { return true; }
  bool keep_alive() override { return false; }

  std::shared_ptr<interface::Renderer> create_renderer() override
  {
    return std::make_shared<OpenglRenderer>();
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::OpenglRendererService> Axgl::use_service()
{
  if (!has_service_type<impl::GlfwWindowService>("window"))
    throw std::runtime_error("GlfwWindowService is missing, it is required for OpenglRendererService.");

  // set glfw context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // create opengl service
  auto opengl_service = std::make_shared<impl::OpenglRendererService>();
  register_service("renderer", opengl_service);

  return opengl_service;
}

NAMESPACE_AXGL_END

// component implementations
#include "axgl/impl/opengl/component/mesh2d.hpp"
#include "axgl/impl/opengl/component/mesh3d.hpp"
