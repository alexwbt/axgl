
#include <axgl/axgl.hpp>
#include <axgl/impl/glfw.hpp>

int main()
{
  axgl::Axgl axgl;
  axgl::configure_glfw(axgl);
  axgl.initialize();

  // create window
  const auto window = axgl.window_service()->create_window();
  window->set_title("Hello Window!");

  axgl.run();
  axgl.terminate();
}
