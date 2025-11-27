#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <axgl/interface/texture.hpp>

namespace axgl::gui
{

enum class TextAlign
{
  BEGIN,
  CENTER,
  END,
};

enum class TextStyle
{
  NORMAL,
  BOLD,
  ITALIC,
};

class Style
{
  glm::vec2 origin{0}, offset{0};
  glm::vec2 size{0}, max_size{0}, min_size{0};
  glm::vec4 padding{0}, margin{0};
  glm::vec4 bg_color{0}, fg_color{0};
  bool hidden = false;
  std::string content;
  std::string font;
  float font_size = 0;
  TextStyle text_style = TextStyle::NORMAL;
  TextAlign horizontal_align = TextAlign::CENTER;
  TextAlign vertical_align = TextAlign::CENTER;
  std::shared_ptr<axgl::Texture> bg_image;
};

} // namespace axgl::gui
