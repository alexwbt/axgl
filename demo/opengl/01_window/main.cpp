
#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>

int main()
{
  axgl::Axgl axgl;

  // register window service
  axgl.use_service<axgl::impl::glfw::WindowService>();
  const auto window_service = axgl.window_service();

  axgl.initialize();

  // create window
  const auto window = window_service->create_window();
  window->set_title("Hello Window!");

  axgl.run();
  axgl.terminate();
}
