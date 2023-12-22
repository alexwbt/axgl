#include "playground.h"

int main()
{
  Playground playground;

  axgl::Gameloop gameloop;
  gameloop.add_component(&playground);
  gameloop.run();
}
