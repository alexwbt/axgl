#pragma once

#include <axgl/axgl.hpp>

#include <axgl/impl/glfw/window.hpp>
#include <axgl/impl/glfw/input.hpp>

/* Service implementations */

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::GlfwWindowService> Axgl::use_service()
{
  glfw::Window::initialize();

  auto glfw_service = std::make_shared<impl::GlfwWindowService>();
  register_service("window", glfw_service);

  return glfw_service;
}

template<>
std::shared_ptr<impl::GlfwInputService> Axgl::use_service()
{
  auto input_service = std::make_shared<impl::GlfwInputService>();
  register_service("input", input_service);

  return input_service;
}

NAMESPACE_AXGL_END
