#pragma once

#include "axgl/namespace.h"

NAMESPACE_WORLD

class Entity
{
public:
  virtual ~Entity() {}
  virtual void initialize();
  virtual void terminate();
  virtual void update();
  virtual void render();
};

NAMESPACE_WORLD_END
