#pragma once

#include <glm/glm.hpp>
#include "axgl/namespace.h"
#include "axgl/world/render_context.h"

NAMESPACE_WORLD

class Entity
{
public:
  virtual ~Entity() {}
  virtual void initialize() {}
  virtual void terminate() {}
  virtual void update() {}
  virtual void render(const RenderContext& context) = 0;
};

NAMESPACE_WORLD_END
