#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/glfw/services/input_service.hpp>
#include <axgl/impl/glfw/services/window_service.hpp>

namespace axgl
{
AXGL_DECLARE_REGISTER_SERVICE(impl::glfw::WindowService)
AXGL_DECLARE_REGISTER_SERVICE(impl::glfw::InputService)

inline void configure_glfw(Axgl& axgl)
{
  axgl.register_service_t<impl::glfw::WindowService>();
  axgl.register_service_t<impl::glfw::InputService>();
}

} // namespace axgl
