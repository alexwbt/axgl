#include <axgl/components/opengl.h>

#include "client.h"
#include "console.h"
#include "playground.h"

int main()
{
  auto window = std::make_shared<axgl::OpenglWindow>(800, 600, "Net Playground");
  window->add_component(std::make_shared<NetClient>("127.0.0.1", 13000));
  window->add_component(std::make_shared<Playground>());

  auto imgui = std::make_shared<axgl::OpenglImgui>(window);
  imgui->add_component(std::make_shared<Console>());
  window->add_component(imgui);

  auto window_event_adapter = std::make_shared<axgl::WindowEventAdapter>(EVENT_TYPE_WINDOW_EVENT_BASE);
  window->set_event_adapter(window_event_adapter);

  axgl::Gameloop gameloop;
  gameloop.add_component(window);

  // run
  gameloop.run();
}
