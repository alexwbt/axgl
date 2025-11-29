#pragma once

#include <memory>

#include <axgl/interface/services/window_service.hpp>

#include <axgl/impl/glfw/window.hpp>
#include <glfw/window.hpp>

namespace axgl::impl::glfw
{

class WindowService final : public axgl::WindowService
{
public:
  void initialize(const Service::Context& context) override { ::glfw::Window::initialize(); }

  void terminate(const Service::Context& context) override { ::glfw::Window::terminate(); }

  void render(const Service::Context& context) override { ::glfw::Window::update_all(); }

  bool running(const Service::Context& context) override { return !::glfw::Window::should_close_all(); }

  bool keep_alive(const Service::Context& context) override { return running(context); }

  std::shared_ptr<axgl::Window> create_window() override
  {
    return std::make_shared<Window>(::glfw::Window::create(800, 600, ""));
  }

  static void set_window_hint(const int hint, const int value) { ::glfw::Window::glfw_window_hint(hint, value); }
};

} // namespace axgl::impl::glfw
