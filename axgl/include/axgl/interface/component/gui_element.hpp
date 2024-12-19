#pragma once

#include "axgl/namespace.hpp"
#include "axgl/interface/realm.hpp"
#include "axgl/interface/renderer.hpp"

NAMESPACE_AXGL_INTERFACE

class GuiElement : public Component
{
public:
  struct Color
  {
    float r = 1;
    float g = 1;
    float b = 1;
    float a = 1;
  };
  struct Space
  {
    float top = 0;
    float down = 0;
    float left = 0;
    float right = 0;
  };
  struct Vector
  {
    float x = 0;
    float y = 0;
  };
  struct Size
  {
    float width = 0;
    float height = 0;
  };
  enum class TextAlign { BEGIN, CENTER, END };
  enum class TextStyle { NORMAL, BOLD, ITALIC };
  struct Properties
  {
    Vector origin, offset;
    Size size, max_size, min_size;
    Space padding, margin;
    Color bg_color, fg_color;
    bool hidden = false;
    bool block_cursor = false;
    std::string content = "";
    std::string font = "";
    float font_size = 0;
    TextStyle text_style = NORMAL;
    TextAlign horizontal_align = CENTER;
    TextAlign vertical_align = CENTER;
    std::shared_ptr<Texture> bg_image;
  };
  Properties props;
  Properties down;
  Properties hover;
  Properties focus;

  virtual void add_child(std::shared_ptr<GuiElement> element) = 0;
  virtual void remove_child(uint32_t id) = 0;

  virtual util::Iterable<std::shared_ptr<GuiElement>> get_children() const = 0;

  virtual uint32_t down_tick() = 0;
  virtual bool hover() = 0;
  virtual bool focus() = 0;
};

NAMESPACE_AXGL_INTERFACE_END
