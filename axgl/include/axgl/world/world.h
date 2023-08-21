#pragma once

#include <list>
#include <memory>

#include "axgl/namespace.h"
#include "axgl/world/entity.h"

NAMESPACE_WORLD

class World
{
  std::list<std::shared_ptr<Entity>> entities_;

public:
  virtual ~World() {}
  virtual void initialize();
  virtual void terminate();
  virtual void update();
  virtual void render();
};

NAMESPACE_WORLD_END
