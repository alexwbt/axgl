#pragma once

#include <axgl/axgl.hpp>
#include <axgl/namespace.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/component/component.hpp>
#include <axgl/impl/glfw/glfw_service.hpp>

#include <glad/glad.h>
#include <spdlog/spdlog.h>

NAMESPACE_AXGL_IMPL

class OpenglRenderer : public interface::Renderer
{
private:
  int clear_bit_ = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
  GLfloat clear_color_r_ = 0.0f;
  GLfloat clear_color_g_ = 0.0f;
  GLfloat clear_color_b_ = 0.0f;
  GLfloat clear_color_a_ = 0.0f;

  std::shared_ptr<interface::Window> window_;

public:
  void before_render() override
  {
    if (!window_) return;
    window_->use();

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
    window_ = std::move(window);
    window_->use();

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      SPDLOG_CRITICAL("Failed to initialize GLAD.");

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
};

class OpenglRendererService : public interface::RendererService
{
public:
  void initialize() override
  {
    // set glfw context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  }

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
  // create opengl service
  auto opengl_service = std::make_shared<impl::OpenglRendererService>();
  register_service("renderer", opengl_service);

  return opengl_service;
}

NAMESPACE_AXGL_END

// component implementations
#include "axgl/impl/opengl/component/mesh.hpp"
