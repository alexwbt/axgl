
#include "terra.hpp"

int main()
{
  auto terra = std::make_shared<Terra>();

  axgl::Gameloop gameloop;
  gameloop.add_component(terra);
}
