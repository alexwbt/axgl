#pragma once

#include <memory>
#include <stdint.h>

#include "axgl/common.hpp"
#include "axgl/interface/window.hpp"

NAMESPACE_AXGL_INTERFACE

enum class InputSource
{
  KEY_SPACE,
  KEY_APOSTROPHE,
  KEY_COMMA,
  KEY_MINUS,
  KEY_PERIOD,
  KEY_SLASH,
  KEY_0,
  KEY_1,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,
  KEY_SEMICOLON,
  KEY_EQUAL,
  KEY_A,
  KEY_B,
  KEY_C,
  KEY_D,
  KEY_E,
  KEY_F,
  KEY_G,
  KEY_H,
  KEY_I,
  KEY_J,
  KEY_K,
  KEY_L,
  KEY_M,
  KEY_N,
  KEY_O,
  KEY_P,
  KEY_Q,
  KEY_R,
  KEY_S,
  KEY_T,
  KEY_U,
  KEY_V,
  KEY_W,
  KEY_X,
  KEY_Y,
  KEY_Z,
  KEY_LEFT_BRACKET,
  KEY_BACKSLASH,
  KEY_RIGHT_BRACKET,
  KEY_GRAVE_ACCENT,
  KEY_WORLD_1,
  KEY_WORLD_2,
  KEY_ESCAPE,
  KEY_ENTER,
  KEY_TAB,
  KEY_BACKSPACE,
  KEY_INSERT,
  KEY_DELETE,
  KEY_RIGHT,
  KEY_LEFT,
  KEY_DOWN,
  KEY_UP,
  KEY_PAGE_UP,
  KEY_PAGE_DOWN,
  KEY_HOME,
  KEY_END,
  KEY_CAPS_LOCK,
  KEY_SCROLL_LOCK,
  KEY_NUM_LOCK,
  KEY_PRINT_SCREEN,
  KEY_PAUSE,
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_F11,
  KEY_F12,
  KEY_F13,
  KEY_F14,
  KEY_F15,
  KEY_F16,
  KEY_F17,
  KEY_F18,
  KEY_F19,
  KEY_F20,
  KEY_F21,
  KEY_F22,
  KEY_F23,
  KEY_F24,
  KEY_F25,
  KEY_KEYPAD_0,
  KEY_KEYPAD_1,
  KEY_KEYPAD_2,
  KEY_KEYPAD_3,
  KEY_KEYPAD_4,
  KEY_KEYPAD_5,
  KEY_KEYPAD_6,
  KEY_KEYPAD_7,
  KEY_KEYPAD_8,
  KEY_KEYPAD_9,
  KEY_KEYPAD_DECIMAL,
  KEY_KEYPAD_DIVIDE,
  KEY_KEYPAD_MULTIPLY,
  KEY_KEYPAD_SUBTRACT,
  KEY_KEYPAD_ADD,
  KEY_KEYPAD_ENTER,
  KEY_KEYPAD_EQUAL,
  KEY_LEFT_SHIFT,
  KEY_LEFT_CONTROL,
  KEY_LEFT_ALT,
  KEY_LEFT_SUPER,
  KEY_RIGHT_SHIFT,
  KEY_RIGHT_CONTROL,
  KEY_RIGHT_ALT,
  KEY_RIGHT_SUPER,
  KEY_MENU,
};

enum class PointerSource
{
  MOUSE_MOVE,
};

enum class CursorMode
{
  LOCKED,
  NORMAL,
};

class Input;
class Pointer;
class InputService : public Service
{
private:
  static uint32_t next_id()
  {
    static uint32_t next_id_ = 1;
    return next_id_++;
  }

public:
  virtual void set_window(std::shared_ptr<Window> window) = 0;
  virtual void add_input(std::shared_ptr<Input> input) = 0;
  virtual void add_pointer(std::shared_ptr<Pointer> pointer) = 0;
  virtual void remove_input(uint32_t id) = 0;
  virtual void remove_pointer(uint32_t id) = 0;

  virtual void set_cursor_mode(CursorMode mode) = 0;

  friend class Input;
  friend class Pointer;
};

class Input
{
public:
  const uint32_t id;
  std::string name;
  InputSource source;
  uint32_t tick;

  Input(const std::string& name, InputSource source) :
    id(InputService::next_id()), name(name), source(source), tick(0)
  {}
};

class Pointer
{
public:
  const uint32_t id;
  std::string name;
  PointerSource source;
  glm::ivec2 position;
  glm::ivec2 delta;
  uint32_t tick;

  Pointer(const std::string& name, PointerSource source) :
    id(InputService::next_id()), name(name), source(source),
    position(0), delta(0), tick(0)
  {}
};

NAMESPACE_AXGL_INTERFACE_END
