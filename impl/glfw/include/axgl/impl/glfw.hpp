#pragma once

#include <axgl/axgl.hpp>

#include <axgl/impl/glfw/services/input_service.hpp>
#include <axgl/impl/glfw/services/window_service.hpp>

namespace axgl
{

template <>
inline std::shared_ptr<impl::GlfwWindowService> Axgl::use_service()
{
  glfw::Window::initialize();

  auto glfw_service = std::make_shared<impl::GlfwWindowService>();
  register_service(DefaultServices::kWindow, glfw_service);

  return glfw_service;
}

template <>
inline std::shared_ptr<impl::GlfwInputService> Axgl::use_service()
{
  auto input_service = std::make_shared<impl::GlfwInputService>();
  register_service(DefaultServices::kInput, input_service);

  return input_service;
}

} // namespace axgl
