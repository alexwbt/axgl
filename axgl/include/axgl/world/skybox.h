#pragma once

#include "axgl/namespace.h"
#include "axgl/world/entity.h"

NAMESPACE_WORLD

class Skybox : public Entity
{
public:
  Skybox();
  void render(const RenderContext& context) override;
};

NAMESPACE_WORLD_END
