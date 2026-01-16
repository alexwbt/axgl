#pragma once

#include <axgl/axgl.hpp>
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  #include <axgl/impl/glfw.hpp>
  #include <axgl/impl/opengl.hpp>
#endif
// #include <axgl/impl/camera/keyboard_3d_free_fly_camera_mode.hpp>
// #include <axgl/util/mesh.hpp>

inline void setup(axgl::Axgl& axgl)
{
  axgl::configure_default(axgl);
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  axgl::configure_glfw(axgl);
  axgl::configure_opengl(axgl);
#endif

  axgl.initialize();

  const auto& window_service = axgl.window_service();
  const auto& input_service = axgl.input_service();
  const auto& renderer_service = axgl.renderer_service();
  // const auto& realm_service = axgl.realm_service();
  // const auto& entity_service = axgl.entity_service();
  // const auto& camera_service = axgl.camera_service();
  // const auto& resource_service = axgl.resource_service();

  // window
  const auto window = window_service->create_window();
  window->set_title("Hello gui!");
  window->set_size(1000, 800);

  // input
  input_service->set_window(window);

  // renderer
  const auto renderer = renderer_service->create_renderer();
  renderer->set_window(window);
  renderer->set_sample_count(8);
  renderer->set_antialiasing(true);
  renderer_service->set_active_renderer(renderer);
}
