#include "playground.h"

int main()
{
  axgl::Gameloop gameloop;
  gameloop.add_component(Playground::make_shared());
  gameloop.run();
}
