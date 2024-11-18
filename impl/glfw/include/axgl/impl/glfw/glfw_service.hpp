#pragma once

#include <axgl/axgl.hpp>
#include <axgl/namespace.hpp>
#include <axgl/interface/window.hpp>
#include <axgl/impl/glfw/glfw_window.hpp>

NAMESPACE_AXGL_IMPL

class GlfwWindow : public interface::Window
{
private:
  std::shared_ptr<glfw::Window> window_;

public:
  GlfwWindow(std::shared_ptr<glfw::Window> window)
    : window_(std::move(window))
  {}

  void set_title(const std::string& title) override
  {
    window_->set_title(title);
  }

  void set_position(uint32_t x, uint32_t y) override {}

  void set_size(uint32_t width, uint32_t height) override {}

  void maximize() override {}

  void hide() override {}

  void show() override {}

  void close() override {}

  void use()
  {
    window_->use();
  }

  void swap_buffers()
  {
    window_->swap_buffers();
  }
};

class GlfwWindowService : public interface::WindowService
{
public:
  void initialize() override {}

  void terminate() override
  {
    glfw::Window::terminate();
  }

  void update() override {}

  void render() override
  {
    glfw::Window::update_all();
  }

  bool running() override
  {
    return !glfw::Window::should_close_all();
  }

  std::shared_ptr<interface::Window> create_window() override
  {
    return std::make_shared<GlfwWindow>(glfw::Window::create(800, 600, ""));
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::GlfwWindowService> Axgl::use_service()
{
  glfw::Window::initialize();

  auto glfw_service = std::make_shared<impl::GlfwWindowService>();
  register_service("window", glfw_service);

  return glfw_service;
}

NAMESPACE_AXGL_END
