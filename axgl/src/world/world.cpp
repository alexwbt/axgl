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

void World::render(const RenderContext& context)
{
  for (auto& entity : entities_)
    entity->render(context);
}

void World::push_entity(std::shared_ptr<Entity> entity)
{
  entities_.push_back(std::move(entity));
}

NAMESPACE_WORLD_END
