#include "axgl/world/world.h"

NAMESPACE_WORLD

void World::initialize()
{
  // for (auto& entity : entities_)
  //   entity->initialize();
}

void World::terminate()
{
  for (auto& entity : entities_)
    entity->terminate();
}

void World::update()
{
  for (auto& entity : entities_)
    entity->update();
}

void World::render()
{
  const Entity::RenderContext info{};

  for (auto& entity : entities_)
    entity->render(info);
}

NAMESPACE_WORLD_END
