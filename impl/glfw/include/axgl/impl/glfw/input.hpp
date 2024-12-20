#pragma once

#include <list>

#include <axgl/axgl.hpp>
#include <axgl/common.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/window.hpp>

#include "glfw/window.hpp"

NAMESPACE_AXGL_IMPL

class GlfwInputService : public interface::InputService
{
private:
  std::shared_ptr<GlfwWindow> window_;
  std::list<std::shared_ptr<interface::Input>> inputs_;
  std::list<std::shared_ptr<interface::Pointer>> pointers_;

  static int to_glfw_keycode(interface::InputSource source)
  {
    using enum interface::InputSource;
    switch (source)
    {
    case KEY_SPACE: return GLFW_KEY_SPACE;
    case KEY_APOSTROPHE: return GLFW_KEY_APOSTROPHE;
    case KEY_COMMA: return GLFW_KEY_COMMA;
    case KEY_MINUS: return GLFW_KEY_MINUS;
    case KEY_PERIOD: return GLFW_KEY_PERIOD;
    case KEY_SLASH: return GLFW_KEY_SLASH;
    case KEY_0: return GLFW_KEY_0;
    case KEY_1: return GLFW_KEY_1;
    case KEY_2: return GLFW_KEY_2;
    case KEY_3: return GLFW_KEY_3;
    case KEY_4: return GLFW_KEY_4;
    case KEY_5: return GLFW_KEY_5;
    case KEY_6: return GLFW_KEY_6;
    case KEY_7: return GLFW_KEY_7;
    case KEY_8: return GLFW_KEY_8;
    case KEY_9: return GLFW_KEY_9;
    case KEY_SEMICOLON: return GLFW_KEY_SEMICOLON;
    case KEY_EQUAL: return GLFW_KEY_EQUAL;
    case KEY_A: return GLFW_KEY_A;
    case KEY_B: return GLFW_KEY_B;
    case KEY_C: return GLFW_KEY_C;
    case KEY_D: return GLFW_KEY_D;
    case KEY_E: return GLFW_KEY_E;
    case KEY_F: return GLFW_KEY_F;
    case KEY_G: return GLFW_KEY_G;
    case KEY_H: return GLFW_KEY_H;
    case KEY_I: return GLFW_KEY_I;
    case KEY_J: return GLFW_KEY_J;
    case KEY_K: return GLFW_KEY_K;
    case KEY_L: return GLFW_KEY_L;
    case KEY_M: return GLFW_KEY_M;
    case KEY_N: return GLFW_KEY_N;
    case KEY_O: return GLFW_KEY_O;
    case KEY_P: return GLFW_KEY_P;
    case KEY_Q: return GLFW_KEY_Q;
    case KEY_R: return GLFW_KEY_R;
    case KEY_S: return GLFW_KEY_S;
    case KEY_T: return GLFW_KEY_T;
    case KEY_U: return GLFW_KEY_U;
    case KEY_V: return GLFW_KEY_V;
    case KEY_W: return GLFW_KEY_W;
    case KEY_X: return GLFW_KEY_X;
    case KEY_Y: return GLFW_KEY_Y;
    case KEY_Z: return GLFW_KEY_Z;
    case KEY_LEFT_BRACKET: return GLFW_KEY_LEFT_BRACKET;
    case KEY_BACKSLASH: return GLFW_KEY_BACKSLASH;
    case KEY_RIGHT_BRACKET: return GLFW_KEY_RIGHT_BRACKET;
    case KEY_GRAVE_ACCENT: return GLFW_KEY_GRAVE_ACCENT;
    case KEY_WORLD_1: return GLFW_KEY_WORLD_1;
    case KEY_WORLD_2: return GLFW_KEY_WORLD_2;
    case KEY_ESCAPE: return GLFW_KEY_ESCAPE;
    case KEY_ENTER: return GLFW_KEY_ENTER;
    case KEY_TAB: return GLFW_KEY_TAB;
    case KEY_BACKSPACE: return GLFW_KEY_BACKSPACE;
    case KEY_INSERT: return GLFW_KEY_INSERT;
    case KEY_DELETE: return GLFW_KEY_DELETE;
    case KEY_RIGHT: return GLFW_KEY_RIGHT;
    case KEY_LEFT: return GLFW_KEY_LEFT;
    case KEY_DOWN: return GLFW_KEY_DOWN;
    case KEY_UP: return GLFW_KEY_UP;
    case KEY_PAGE_UP: return GLFW_KEY_PAGE_UP;
    case KEY_PAGE_DOWN: return GLFW_KEY_PAGE_DOWN;
    case KEY_HOME: return GLFW_KEY_HOME;
    case KEY_END: return GLFW_KEY_END;
    case KEY_CAPS_LOCK: return GLFW_KEY_CAPS_LOCK;
    case KEY_SCROLL_LOCK: return GLFW_KEY_SCROLL_LOCK;
    case KEY_NUM_LOCK: return GLFW_KEY_NUM_LOCK;
    case KEY_PRINT_SCREEN: return GLFW_KEY_PRINT_SCREEN;
    case KEY_PAUSE: return GLFW_KEY_PAUSE;
    case KEY_F1: return GLFW_KEY_F1;
    case KEY_F2: return GLFW_KEY_F2;
    case KEY_F3: return GLFW_KEY_F3;
    case KEY_F4: return GLFW_KEY_F4;
    case KEY_F5: return GLFW_KEY_F5;
    case KEY_F6: return GLFW_KEY_F6;
    case KEY_F7: return GLFW_KEY_F7;
    case KEY_F8: return GLFW_KEY_F8;
    case KEY_F9: return GLFW_KEY_F9;
    case KEY_F10: return GLFW_KEY_F10;
    case KEY_F11: return GLFW_KEY_F11;
    case KEY_F12: return GLFW_KEY_F12;
    case KEY_F13: return GLFW_KEY_F13;
    case KEY_F14: return GLFW_KEY_F14;
    case KEY_F15: return GLFW_KEY_F15;
    case KEY_F16: return GLFW_KEY_F16;
    case KEY_F17: return GLFW_KEY_F17;
    case KEY_F18: return GLFW_KEY_F18;
    case KEY_F19: return GLFW_KEY_F19;
    case KEY_F20: return GLFW_KEY_F20;
    case KEY_F21: return GLFW_KEY_F21;
    case KEY_F22: return GLFW_KEY_F22;
    case KEY_F23: return GLFW_KEY_F23;
    case KEY_F24: return GLFW_KEY_F24;
    case KEY_F25: return GLFW_KEY_F25;
    case KEY_KEYPAD_0: return GLFW_KEY_KP_0;
    case KEY_KEYPAD_1: return GLFW_KEY_KP_1;
    case KEY_KEYPAD_2: return GLFW_KEY_KP_2;
    case KEY_KEYPAD_3: return GLFW_KEY_KP_3;
    case KEY_KEYPAD_4: return GLFW_KEY_KP_4;
    case KEY_KEYPAD_5: return GLFW_KEY_KP_5;
    case KEY_KEYPAD_6: return GLFW_KEY_KP_6;
    case KEY_KEYPAD_7: return GLFW_KEY_KP_7;
    case KEY_KEYPAD_8: return GLFW_KEY_KP_8;
    case KEY_KEYPAD_9: return GLFW_KEY_KP_9;
    case KEY_KEYPAD_DECIMAL: return GLFW_KEY_KP_DECIMAL;
    case KEY_KEYPAD_DIVIDE: return GLFW_KEY_KP_DIVIDE;
    case KEY_KEYPAD_MULTIPLY: return GLFW_KEY_KP_MULTIPLY;
    case KEY_KEYPAD_SUBTRACT: return GLFW_KEY_KP_SUBTRACT;
    case KEY_KEYPAD_ADD: return GLFW_KEY_KP_ADD;
    case KEY_KEYPAD_ENTER: return GLFW_KEY_KP_ENTER;
    case KEY_KEYPAD_EQUAL: return GLFW_KEY_KP_EQUAL;
    case KEY_LEFT_SHIFT: return GLFW_KEY_LEFT_SHIFT;
    case KEY_LEFT_CONTROL: return GLFW_KEY_LEFT_CONTROL;
    case KEY_LEFT_ALT: return GLFW_KEY_LEFT_ALT;
    case KEY_LEFT_SUPER: return GLFW_KEY_LEFT_SUPER;
    case KEY_RIGHT_SHIFT: return GLFW_KEY_RIGHT_SHIFT;
    case KEY_RIGHT_CONTROL: return GLFW_KEY_RIGHT_CONTROL;
    case KEY_RIGHT_ALT: return GLFW_KEY_RIGHT_ALT;
    case KEY_RIGHT_SUPER: return GLFW_KEY_RIGHT_SUPER;
    case KEY_MENU: return GLFW_KEY_MENU;
    default: return -1;
    }
  }

public:
  void set_window(std::shared_ptr<interface::Window> window)
  {
    window_ = dynamic_pointer_cast<GlfwWindow>(window);
#ifdef AXGL_DEBUG
    if (!window_)
      throw std::runtime_error(
        "The provided window is not a valid GlfwWindow instance. "
        "GlfwWindow is required for GlfwInputService.");
#endif
  }

  void set_cursor_mode(interface::CursorMode mode) override
  {
    if (!window_)
#ifdef AXGL_DEBUG
      throw std::runtime_error("GlfwWindow is not set.");
#else
      return;
#endif
    window_->use();
    using enum interface::CursorMode;
    switch (mode)
    {
    case LOCKED:
      window_->glfw_window()->set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      break;
    case NORMAL:
      window_->glfw_window()->set_input_mode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      break;
    }
  }

  void add_input(std::shared_ptr<interface::Input> input) override
  {
    inputs_.push_back(std::move(input));
  }

  void add_pointer(std::shared_ptr<interface::Pointer> pointer) override
  {
    pointers_.push_back(std::move(pointer));
  }

  void remove_input(uint32_t id) override
  {
    inputs_.remove_if([id](const auto& input) { return input->id == id; });
  }

  void remove_pointer(uint32_t id) override
  {
    pointers_.remove_if([id](const auto& pointer) { return pointer->id == id; });
  }

  void update() override
  {
    if (!window_)
      return;

    for (auto& input : inputs_)
    {
      if (window_->glfw_window()->key_pressed(to_glfw_keycode(input->source)))
        input->tick++;
      else
        input->tick = 0;
    }

    for (auto& pointer : pointers_)
    {
      using enum interface::PointerSource;
      switch (pointer->source)
      {
      case MOUSE_MOVE:
        auto pos = window_->glfw_window()->get_mouse_pos();
        pointer->delta = pos - pointer->position;
        pointer->position = pos;
        pointer->tick++;
        break;
      }
    }
  }
};

NAMESPACE_AXGL_IMPL_END
