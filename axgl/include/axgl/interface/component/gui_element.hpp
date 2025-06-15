#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/renderer.hpp>

NAMESPACE_AXGL_INTERFACE

namespace component
{
  class GuiElement : public Component
  {
  public:
    enum class TextAlign { BEGIN, CENTER, END };
    enum class TextStyle { NORMAL, BOLD, ITALIC };
    struct Properties
    {
      glm::vec2 origin{ 0 }, offset{ 0 };
      glm::vec2 size{ 0 }, max_size{ 0 }, min_size{ 0 };
      glm::vec4 padding{ 0 }, margin{ 0 };
      glm::vec4 bg_color{ 0 }, fg_color{ 0 };
      bool hidden = false;
      bool block_cursor = false;
      std::string content = "";
      std::string font = "";
      float font_size = 0;
      TextStyle text_style = TextStyle::NORMAL;
      TextAlign horizontal_align = TextAlign::CENTER;
      TextAlign vertical_align = TextAlign::CENTER;
      std::shared_ptr<Texture> bg_image;
    };
    Properties props;
    Properties down;
    Properties hover;
    Properties focus;

    virtual uint32_t down_tick() = 0;
    virtual bool hovering() = 0;
    virtual bool focused() = 0;
  };
}

NAMESPACE_AXGL_INTERFACE_END
