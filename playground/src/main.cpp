#include "playground.h"

int main()
{
  auto playground = std::make_shared<Playground>();

  gl::Gameloop gameloop;
  gameloop.add_component(playground);
  gameloop.run();
}
