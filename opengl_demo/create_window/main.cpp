
#include <axgl/axgl.hpp>
#include <axgl/impl/glfw/glfw_service.hpp>

int main()
{
  axgl::Axgl axgl;

  // register glfw service
  auto glfw = axgl.use_service<axgl::impl::GlfwService>();
  // create window
  auto window = glfw->create_window();
  window->set_title("AXGL Window!");

  axgl.run();
}
