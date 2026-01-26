#include <axgl/axgl.hpp>
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  #include <axgl/impl/glfw.hpp>
  #include <axgl/impl/opengl.hpp>
#endif

#include "service/input_manager.hpp"
#include "service/playground.hpp"

int main()
{
  axgl::Axgl axgl;
  axgl::configure_default(axgl);
#ifdef AXGL_DEMO_USE_OPENGL_IMPL
  axgl::configure_glfw(axgl);
  axgl::configure_opengl(axgl);
#endif
  axgl.register_service("input_manager", axgl::create_ptr<InputManager>());
  axgl.register_service("playground", axgl::create_ptr<Playground>());
  axgl.initialize();

  const auto& window_service = axgl.window_service();
  const auto& input_service = axgl.input_service();
  const auto& renderer_service = axgl.renderer_service();
  // window
  const auto window = window_service->create_window();
  window->set_title("Demo Playground");
  window->set_size(1200, 800);
  // input
  input_service->set_window(window);
  // renderer
  const auto renderer = renderer_service->create_renderer();
  renderer->set_window(window);
  renderer->set_sample_count(8);
  renderer->set_antialiasing(true);
  renderer_service->set_active_renderer(renderer);

  axgl.run();
  axgl.terminate();
}
