#include <axgl/components/opengl.h>

#include "client.h"
#include "console.h"
#include "playground.h"


int main()
{
  auto window = std::make_shared<axgl::OpenglWindow>();
  window->add_component(Playground::make_shared(window));
  window->add_component(std::make_shared<NetClient>("127.0.0.1", 13000));

  auto imgui = std::make_shared<axgl::OpenglImgui>(window);
  imgui->add_component(std::make_shared<Console>());
  window->add_component(imgui);

  axgl::Gameloop gameloop;
  gameloop.add_component(window);

  // run
  gameloop.run();
}
