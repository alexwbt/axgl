#include <axgl/components/opengl.h>

#include "client.h"
#include "console.h"
#include "playground.h"


int main()
{
  // net client
  NetClient client("127.0.0.1", 13000);
  std::thread client_thread([&client]() { client.start(); });

  // gameloop
  axgl::Gameloop gameloop;

  // window
  axgl::OpenglWindow window;
  gameloop.add_component(&window);

  // imgui
  axgl::OpenglImgui imgui(window);
  window.add_component(&imgui);

  // console
  Console console(client);
  imgui.add_component(&console);

  // game
  Playground playground(window);
  window.add_component(&playground);

  // run
  gameloop.run();

  // end
  client.stop();
  client_thread.join();
}
