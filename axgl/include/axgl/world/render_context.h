#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "axgl/namespace.h"
#include "axgl/world/camera.h"

NAMESPACE_WORLD

struct RenderContext final
{
  uint32_t view_width;
  uint32_t view_height;

  glm::mat4 projection;
  glm::mat4 view;
  glm::mat4 pv;

  std::shared_ptr<Camera> camera;
};

NAMESPACE_WORLD_END
