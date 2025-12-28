#pragma once

#include <axgl/common.hpp>

namespace axgl::gui
{
//
// enum class TextAlign
// {
//   kLeft,
//   kRight,
//   kCenter,
// };

struct Property
{
  glm::vec2 position{0.0f};
  glm::vec2 size{0.0f};
  glm::vec4 color{0.0f};
  // layout
  // glm::vec4 margin{0.0f};
  // glm::vec4 padding{0.0f};
  // content
  // std::string content;
  // std::string font;
  // glm::vec4 font_color{1.0f};
  // float font_size = 16.0f;
  // float line_height = 1.5f;
  // TextAlign text_align = TextAlign::kCenter;
};

} // namespace axgl::gui
