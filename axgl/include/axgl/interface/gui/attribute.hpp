#pragma once

#include <axgl/common.hpp>

namespace axgl::gui
{

struct Attribute
{
  glm::vec2 position{0.0f};
  glm::vec2 size{0.0f};
  glm::vec4 margin{0.0f};
  glm::vec4 padding{0.0f};
  glm::vec4 color{0.0f};
  glm::vec4 border_color{0.0f};
  float border_width = 0.0f;
  float border_radius = 0.0f;
};

} // namespace axgl::gui
