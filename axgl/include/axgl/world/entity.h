#pragma once

#include <glm/glm.hpp>
#include "axgl/namespace.h"

NAMESPACE_WORLD

class Entity
{
public:
  struct RenderContext final
  {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 pv;
  };

  virtual ~Entity() {}
  virtual void initialize() {}
  virtual void terminate() {}
  virtual void update() {}
  virtual void render(const RenderContext& context) = 0;
};

NAMESPACE_WORLD_END
