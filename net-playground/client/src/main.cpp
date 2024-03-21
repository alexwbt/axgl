#include <axgl/components/opengl.h>

#include "client.hpp"
#include "playground.hpp"
#include "console/console.hpp"

int main()
{
  auto window = std::make_shared<axgl::OpenglWindow>(800, 600, "Net Playground");
  window->add_component(std::make_shared<NetClient>(std::make_shared<asio::io_context>()));
  window->add_component(std::make_shared<Playground>());

  auto imgui = std::make_shared<axgl::OpenglImgui>(window);
  imgui->add_component(std::make_shared<console::Console>());
  window->add_component(imgui);

  auto window_event_adapter = std::make_shared<axgl::WindowEventAdapter>(EVENT_TYPE_WINDOW_EVENT_BASE);
  window->set_event_adapter(window_event_adapter);

  axgl::Gameloop gameloop;
  gameloop.add_component(window);

  // run
  gameloop.run();
}
