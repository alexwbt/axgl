#pragma once

#include "axgl/namespace.hpp"
#include "axgl/interface/realm.hpp"

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
  struct Dimension
  {
    float width = 0;
    float height = 0;
  };
  struct Style
  {
    Vector origin;
    Vector offset;
    Dimension dimention;
    Space padding;
    Space margin;
    Color background_color;
    bool hidden = false;
    bool block_cursor = false;
  };
  Style style;

  virtual void add_child(const std::string& id, std::shared_ptr<GuiElement> element) = 0;
  virtual void remove_child(const std::string& id) = 0;

  virtual std::shared_ptr<GuiElement> get_child(const std::string& id) = 0;
  virtual util::Iterable<std::shared_ptr<GuiElement>> get_children() const = 0;

  virtual uint32_t down_tick() = 0;
  virtual bool hover() = 0;
};

class GuiText : public GuiElement
{
public:
  enum class TextAlign { BEGIN, CENTER, END };
  enum class TextStyle { NORMAL, BOLD, ITALIC };
  struct Style
  {
    std::string text = "";
    std::string font = "";
    float font_size = 0;
    TextStyle text_style = NORMAL;
    TextAlign horizontal_align = CENTER;
    TextAlign vertical_align = CENTER;
    GuiElement::Color text_color;
  };
  Style text_style;
};

class GuiButton : public GuiElement
{
public:
  GuiElement::Style hover_style;
  GuiElement::Style down_style;

  virtual void set_text(std::shared_ptr<GuiText> text) = 0;
  virtual void set_text_hover(std::shared_ptr<GuiText> text) = 0;
  virtual void set_text_down(std::shared_ptr<GuiText> text) = 0;
};

class GuiTextInput : public GuiElement
{
public:
};

class GuiImage : public GuiElement
{
public:
};

NAMESPACE_AXGL_INTERFACE_END
