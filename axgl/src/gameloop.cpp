#include "axgl/gameloop.h"

#include <chrono>

NAMESPACE_GL

void Gameloop::add_component(std::shared_ptr<Component> component)
{
  components_.push_back(std::move(component));
}

void Gameloop::initialize()
{
  for (const auto& component : components_)
    component->initialize();
}

void Gameloop::terminate()
{
  for (auto it = components_.rbegin(); it != components_.rend(); ++it)
    (*it)->terminate();
}

void Gameloop::update()
{
  for (const auto& component : components_)
    if (component->alive())
      component->update();
}

void Gameloop::render()
{
  for (const auto& component : components_)
    if (component->alive())
      component->render();
}

bool Gameloop::alive()
{
  for (const auto& component : components_)
    if (component->alive())
      return true;
  return false;
}

void Gameloop::run()
{
  initialize();

  constexpr int64_t kOneSecond = 1000000000;
  constexpr double kTimeStep = kOneSecond / 60.0;

  auto start_time = std::chrono::high_resolution_clock::now();
  double delta_time = 0.0;

  while (alive())
  {
    auto now = std::chrono::high_resolution_clock::now();
    delta_time += (double)std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time).count() / kTimeStep;
    start_time = now;

    while (delta_time >= 1)
    {
      update();
      delta_time--;
    }

    render();
  }

  terminate();
}

NAMESPACE_GL_END
