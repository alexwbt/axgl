#pragma once

#include <list>

#include <axgl/common.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/pointer.hpp>
#include <axgl/interface/services/input_service.hpp>

#include <axgl/impl/glfw/window.hpp>

namespace axgl::impl::glfw
{

class InputService : public axgl::InputService
{
  axgl::ptr_t<axgl::impl::glfw::Window> window_;
  std::list<axgl::ptr_t<axgl::Input>> inputs_;
  std::list<axgl::ptr_t<axgl::Pointer>> pointers_;

  static int to_glfw_keycode(const axgl::Input::Source source)
  {
    using enum axgl::Input::Source;
    switch (source)
    {
    case kKeySpace: return GLFW_KEY_SPACE;
    case kKeyApostrophe: return GLFW_KEY_APOSTROPHE;
    case kKeyComma: return GLFW_KEY_COMMA;
    case kKeyMinus: return GLFW_KEY_MINUS;
    case kKeyPeriod: return GLFW_KEY_PERIOD;
    case kKeySlash: return GLFW_KEY_SLASH;
    case kKey0: return GLFW_KEY_0;
    case kKey1: return GLFW_KEY_1;
    case kKey2: return GLFW_KEY_2;
    case kKey3: return GLFW_KEY_3;
    case kKey4: return GLFW_KEY_4;
    case kKey5: return GLFW_KEY_5;
    case kKey6: return GLFW_KEY_6;
    case kKey7: return GLFW_KEY_7;
    case kKey8: return GLFW_KEY_8;
    case kKey9: return GLFW_KEY_9;
    case kKeySemicolon: return GLFW_KEY_SEMICOLON;
    case kKeyEqual: return GLFW_KEY_EQUAL;
    case kKeyA: return GLFW_KEY_A;
    case kKeyB: return GLFW_KEY_B;
    case kKeyC: return GLFW_KEY_C;
    case kKeyD: return GLFW_KEY_D;
    case kKeyE: return GLFW_KEY_E;
    case kKeyF: return GLFW_KEY_F;
    case kKeyG: return GLFW_KEY_G;
    case kKeyH: return GLFW_KEY_H;
    case kKeyI: return GLFW_KEY_I;
    case kKeyJ: return GLFW_KEY_J;
    case kKeyK: return GLFW_KEY_K;
    case kKeyL: return GLFW_KEY_L;
    case kKeyM: return GLFW_KEY_M;
    case kKeyN: return GLFW_KEY_N;
    case kKeyO: return GLFW_KEY_O;
    case kKeyP: return GLFW_KEY_P;
    case kKeyQ: return GLFW_KEY_Q;
    case kKeyR: return GLFW_KEY_R;
    case kKeyS: return GLFW_KEY_S;
    case kKeyT: return GLFW_KEY_T;
    case kKeyU: return GLFW_KEY_U;
    case kKeyV: return GLFW_KEY_V;
    case kKeyW: return GLFW_KEY_W;
    case kKeyX: return GLFW_KEY_X;
    case kKeyY: return GLFW_KEY_Y;
    case kKeyZ: return GLFW_KEY_Z;
    case kKeyLeftBracket: return GLFW_KEY_LEFT_BRACKET;
    case kKeyBackslash: return GLFW_KEY_BACKSLASH;
    case kKeyRightBracket: return GLFW_KEY_RIGHT_BRACKET;
    case kKeyGraveAccent: return GLFW_KEY_GRAVE_ACCENT;
    case kKeyWorld1: return GLFW_KEY_WORLD_1;
    case kKeyWorld2: return GLFW_KEY_WORLD_2;
    case kKeyEscape: return GLFW_KEY_ESCAPE;
    case kKeyEnter: return GLFW_KEY_ENTER;
    case kKeyTab: return GLFW_KEY_TAB;
    case kKeyBackspace: return GLFW_KEY_BACKSPACE;
    case kKeyInsert: return GLFW_KEY_INSERT;
    case kKeyDelete: return GLFW_KEY_DELETE;
    case kKeyRight: return GLFW_KEY_RIGHT;
    case kKeyLeft: return GLFW_KEY_LEFT;
    case kKeyDown: return GLFW_KEY_DOWN;
    case kKeyUp: return GLFW_KEY_UP;
    case kKeyPageUp: return GLFW_KEY_PAGE_UP;
    case kKeyPageDown: return GLFW_KEY_PAGE_DOWN;
    case kKeyHome: return GLFW_KEY_HOME;
    case kKeyEnd: return GLFW_KEY_END;
    case kKeyCapsLock: return GLFW_KEY_CAPS_LOCK;
    case kKeyScrollLock: return GLFW_KEY_SCROLL_LOCK;
    case kKeyNumLock: return GLFW_KEY_NUM_LOCK;
    case kKeyPrintScreen: return GLFW_KEY_PRINT_SCREEN;
    case kKeyPause: return GLFW_KEY_PAUSE;
    case kKeyF1: return GLFW_KEY_F1;
    case kKeyF2: return GLFW_KEY_F2;
    case kKeyF3: return GLFW_KEY_F3;
    case kKeyF4: return GLFW_KEY_F4;
    case kKeyF5: return GLFW_KEY_F5;
    case kKeyF6: return GLFW_KEY_F6;
    case kKeyF7: return GLFW_KEY_F7;
    case kKeyF8: return GLFW_KEY_F8;
    case kKeyF9: return GLFW_KEY_F9;
    case kKeyF10: return GLFW_KEY_F10;
    case kKeyF11: return GLFW_KEY_F11;
    case kKeyF12: return GLFW_KEY_F12;
    case kKeyF13: return GLFW_KEY_F13;
    case kKeyF14: return GLFW_KEY_F14;
    case kKeyF15: return GLFW_KEY_F15;
    case kKeyF16: return GLFW_KEY_F16;
    case kKeyF17: return GLFW_KEY_F17;
    case kKeyF18: return GLFW_KEY_F18;
    case kKeyF19: return GLFW_KEY_F19;
    case kKeyF20: return GLFW_KEY_F20;
    case kKeyF21: return GLFW_KEY_F21;
    case kKeyF22: return GLFW_KEY_F22;
    case kKeyF23: return GLFW_KEY_F23;
    case kKeyF24: return GLFW_KEY_F24;
    case kKeyF25: return GLFW_KEY_F25;
    case kKeyKeypad0: return GLFW_KEY_KP_0;
    case kKeyKeypad1: return GLFW_KEY_KP_1;
    case kKeyKeypad2: return GLFW_KEY_KP_2;
    case kKeyKeypad3: return GLFW_KEY_KP_3;
    case kKeyKeypad4: return GLFW_KEY_KP_4;
    case kKeyKeypad5: return GLFW_KEY_KP_5;
    case kKeyKeypad6: return GLFW_KEY_KP_6;
    case kKeyKeypad7: return GLFW_KEY_KP_7;
    case kKeyKeypad8: return GLFW_KEY_KP_8;
    case kKeyKeypad9: return GLFW_KEY_KP_9;
    case kKeyKeypadDecimal: return GLFW_KEY_KP_DECIMAL;
    case kKeyKeypadDivide: return GLFW_KEY_KP_DIVIDE;
    case kKeyKeypadMultiply: return GLFW_KEY_KP_MULTIPLY;
    case kKeyKeypadSubtract: return GLFW_KEY_KP_SUBTRACT;
    case kKeyKeypadAdd: return GLFW_KEY_KP_ADD;
    case kKeyKeypadEnter: return GLFW_KEY_KP_ENTER;
    case kKeyKeypadEqual: return GLFW_KEY_KP_EQUAL;
    case kKeyLeftShift: return GLFW_KEY_LEFT_SHIFT;
    case kKeyLeftControl: return GLFW_KEY_LEFT_CONTROL;
    case kKeyLeftAlt: return GLFW_KEY_LEFT_ALT;
    case kKeyLeftSuper: return GLFW_KEY_LEFT_SUPER;
    case kKeyRightShift: return GLFW_KEY_RIGHT_SHIFT;
    case kKeyRightControl: return GLFW_KEY_RIGHT_CONTROL;
    case kKeyRightAlt: return GLFW_KEY_RIGHT_ALT;
    case kKeyRightSuper: return GLFW_KEY_RIGHT_SUPER;
    case kKeyMenu: return GLFW_KEY_MENU;
    default: return -1;
    }
  }

public:
  void set_window(const axgl::ptr_t<axgl::Window> window) override
  {
    window_ = dynamic_pointer_cast<axgl::impl::glfw::Window>(window);
#ifdef AXGL_DEBUG
    if (!window_)
      throw std::runtime_error(
        "The provided window is not a valid GlfwWindow instance. "
        "GlfwWindow is required for GlfwInputService.");
#endif
  }

  void set_cursor_mode(const axgl::InputService::CursorMode mode) override
  {
    if (!window_)
#ifdef AXGL_DEBUG
      throw std::runtime_error("GlfwWindow is not set.");
#else
      return;
#endif
    window_->use();
    using enum axgl::InputService::CursorMode;
    switch (mode)
    {
    case kLocked: window_->glfw_window()->set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED); break;
    case kNormal: window_->glfw_window()->set_input_mode(GLFW_CURSOR, GLFW_CURSOR_NORMAL); break;
    }
  }

  void add_input(axgl::ptr_t<axgl::Input> input) override { inputs_.push_back(std::move(input)); }

  void add_pointer(axgl::ptr_t<axgl::Pointer> pointer) override { pointers_.push_back(std::move(pointer)); }

  void remove_input(std::uint32_t id) override
  {
    inputs_.remove_if([id](const auto& input)
    {
      return input->id == id;
    });
  }

  void remove_pointer(std::uint32_t id) override
  {
    pointers_.remove_if([id](const auto& pointer)
    {
      return pointer->id == id;
    });
  }

  void update(const axgl::Service::Context& context) override
  {
    if (!window_)
      return;

    for (const auto& input : inputs_)
    {
      if (window_->glfw_window()->key_pressed(to_glfw_keycode(input->source)))
        input->tick++;
      else
        input->tick = 0;
    }

    for (const auto& pointer : pointers_)
    {
      using enum axgl::Pointer::Source;
      switch (pointer->source)
      {
      case kMouseMove:
        auto pos = window_->glfw_window()->get_mouse_pos();
        pointer->delta = pos - pointer->position;
        pointer->position = pos;
        pointer->tick++;
        break;
      }
    }
  }
};

} // namespace axgl::impl::glfw

