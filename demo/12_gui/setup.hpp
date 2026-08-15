#pragma once

#include <axgl/axgl.hpp>
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  #include <axgl/impl/glfw.hpp>
  #include <axgl/impl/opengl.hpp>
#endif

inline void setup(axgl::Axgl& axgl)
{
  axgl::configure_default(axgl);
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  axgl::configure_glfw(axgl);
  axgl::configure_opengl(axgl);
#endif

  const auto& window_service = axgl.window_service();
  const auto& input_service = axgl.input_service();
  const auto& renderer_service = axgl.renderer_service();

  // window
  const auto window = window_service->create_window();
  window->set_title("Hello gui!");
  window->set_size(1000, 800);

  // input
  input_service->set_window(window);

  // renderer
  const auto renderer = renderer_service->create_renderer();
  renderer->set_window(window);
  renderer->set_enable_msaa(true);
  renderer->set_msaa_sample_count(8);
  renderer_service->set_active_renderer(renderer);
}
