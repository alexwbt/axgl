#pragma once

#include "axgl/namespace.h"

NAMESPACE_WORLD

class Skybox : public Entity
{
public:
  Skybox();
  void render() override;
};

NAMESPACE_WORLD_END
