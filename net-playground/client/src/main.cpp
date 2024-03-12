#include <axgl/components/opengl.h>

#include "client.h"
#include "console.h"
#include "playground.h"


int main()
{
  // gameloop
  axgl::Gameloop gameloop;

  // window
  axgl::OpenglWindow window;
  gameloop.add_component(&window);

  // imgui
  axgl::OpenglImgui imgui(window);
  window.add_component(&imgui);

  // game
  Playground playground(window);
  window.add_component(&playground);

  // console
  Console console;
  imgui.add_component(&console);

  // run
  gameloop.run();
}
