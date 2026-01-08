#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/services/window_service.hpp>

#include <axgl/impl/glfw/window.hpp>
#include <glfw/window.hpp>

namespace axgl::impl::glfw
{

class WindowService final : public axgl::WindowService
{
public:
  void initialize(const axgl::Service::Context& context) override { ::glfw::Window::initialize(); }

  void terminate(const axgl::Service::Context& context) override { ::glfw::Window::terminate(); }

  void render(const axgl::Service::Context& context) override { ::glfw::Window::update_all(); }

  bool running(const axgl::Service::Context& context) override { return !::glfw::Window::should_close_all(); }

  bool keep_alive(const axgl::Service::Context& context) override { return running(context); }

  axgl::ptr_t<axgl::Window> create_window() override
  {
    return axgl::create_ptr<Window>(::glfw::Window::create(800, 600, ""));
  }

  static void set_window_hint(const int hint, const int value) { ::glfw::Window::set_hint(hint, value); }
  static void set_window_hint(const int hint, const std::string& value) { ::glfw::Window::set_hint(hint, value); }
};

} // namespace axgl::impl::glfw
