#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <axgl/common.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/impl/glfw/window.hpp>
#include <axgl/util/string.hpp>

#include <axgl/impl/opengl/texture.hpp>
#include <axgl/impl/opengl/material.hpp>

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
    if (size.x != window_width_ || size.y != window_height_)
    {
      window_width_ = size.x;
      window_height_ = size.y;
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
    window_ = std::dynamic_pointer_cast<GlfwWindow>(std::move(window));
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
    if (!initialized_glad_ && !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      SPDLOG_CRITICAL("Failed to initialize GLAD.");
    initialized_glad_ = true;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
  }

  glm::ivec2 viewport() const override
  {
    return { window_width_, window_height_ };
  }
};

class OpenglRendererService : public interface::RendererService
{
public:
  std::shared_ptr<interface::Renderer> create_renderer() override
  {
    return std::make_shared<OpenglRenderer>();
  }

  std::shared_ptr<interface::Texture> create_texture() override
  {
    return std::make_shared<OpenglTexture>();
  }

  std::shared_ptr<interface::Material> create_material(const std::string& type) override
  {
    if (type == "2d")
      return std::make_shared<OpenglDefault2DMaterial>();

    if (type == "default")
      return std::make_shared<OpenglDefaultMaterial>();

#ifdef AXGL_DEBUG
    throw std::runtime_error("Unsupported material type: " + type);
#else
    return nullptr;
#endif
  }
};

NAMESPACE_AXGL_IMPL_END
