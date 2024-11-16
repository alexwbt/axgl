
#include <axgl/axgl.hpp>
#include <axgl/impl/glfw/glfw_service.hpp>

#include <spdlog/spdlog.h>

int main()
{
  try
  {
    axgl::Axgl axgl;

    auto glfw = axgl.use_service<axgl::impl::GlfwService>();
    auto window = glfw->create_window();
    window->set_title("AXGL Window!");

    axgl.run();
  }
  catch (const std::exception& e)
  {
    SPDLOG_ERROR(e.what());
  }
}
