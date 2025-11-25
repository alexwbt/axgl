#pragma once

#include <glm/glm.hpp>

namespace axgl
{

struct Transform final
{
  glm::vec3 scale{1.0f};
  glm::vec3 rotation{0.0f};
  glm::vec3 position{0.0f};
};

} // namespace axgl
