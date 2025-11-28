#pragma once

#include <axgl/axgl.hpp>

#include <axgl/impl/glfw/services/input_service.hpp>
#include <axgl/impl/glfw/services/window_service.hpp>

namespace axgl
{

template <>
inline std::shared_ptr<impl::glfw::WindowService> Axgl::use_service()
{
  glfw::Window::initialize();

  auto service = std::make_shared<impl::glfw::WindowService>();
  register_service(DefaultServices::kWindow, service);

  return service;
}

template <>
inline std::shared_ptr<impl::glfw::InputService> Axgl::use_service()
{
  auto service = std::make_shared<impl::glfw::InputService>();
  register_service(DefaultServices::kInput, service);

  return service;
}

} // namespace axgl
