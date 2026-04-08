#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/services/window_service.hpp>

#include <axgl/impl/glfw/window.hpp>
#include <axgl/impl/service_base.hpp>

#include <glfw/window.hpp>

namespace axgl::impl::glfw
{

class WindowService final : virtual public axgl::WindowService, public axgl::impl::ServiceBase
{
public:
  void initialize() override { ::glfw::Window::initialize(); }

  void terminate() override { ::glfw::Window::terminate(); }

  void render() override { ::glfw::Window::update_all(); }

  bool running() override { return !::glfw::Window::should_close_all(); }

  bool keep_alive() override { return running(); }

  axgl::ptr_t<axgl::Window> create_window() override
  {
    return axgl::create_ptr<Window>(::glfw::Window::create(800, 600, ""));
  }

  static void set_window_hint(const int hint, const int value) { ::glfw::Window::set_hint(hint, value); }
  static void set_window_hint(const int hint, const std::string& value) { ::glfw::Window::set_hint(hint, value); }
};

} // namespace axgl::impl::glfw
