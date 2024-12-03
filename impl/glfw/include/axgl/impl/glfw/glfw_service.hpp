#pragma once

#include <unordered_map>

#include <axgl/axgl.hpp>
#include <axgl/namespace.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/window.hpp>
#include <axgl/impl/glfw/glfw_window.hpp>

NAMESPACE_AXGL_IMPL

class GlfwWindow : public interface::Window
{
private:
  std::shared_ptr<glfw::Window> window_;

public:
  GlfwWindow(std::shared_ptr<glfw::Window> window)
    : window_(std::move(window))
  {}

  void set_title(const std::string& title) override
  {
    if (!window_->is_destroyed())
      window_->set_title(title);
  }

  void set_position(uint32_t x, uint32_t y) override
  {
    if (!window_->is_destroyed())
      window_->set_position(x, y);
  }

  void set_size(uint32_t width, uint32_t height) override
  {
    if (!window_->is_destroyed())
      window_->set_size(width, height);
  }

  void maximize() override {}

  void hide() override {}

  void show() override {}

  void close() override {}

  bool ready() const override
  {
    return !window_->is_destroyed();
  }

  void use() const override
  {
    if (!window_->is_destroyed())
      window_->use();
  }

  void swap_buffers() const override
  {
    if (!window_->is_destroyed())
      window_->swap_buffers();
  }

  glm::ivec2 get_size() const override
  {
    return window_->get_size();
  }

public:
  auto glfw_window() const
  {
    return window_;
  }
};

class GlfwWindowService : public interface::WindowService
{
public:
  void terminate() override
  {
    glfw::Window::terminate();
  }

  void render() override
  {
    glfw::Window::update_all();
  }

  bool running() override
  {
    return !glfw::Window::should_close_all();
  }

  bool keep_alive() override
  {
    return running();
  }

  std::shared_ptr<interface::Window> create_window() override
  {
    return std::make_shared<GlfwWindow>(glfw::Window::create(800, 600, ""));
  }
};

class GlfwInputService : public interface::InputService
{
private:
  std::shared_ptr<GlfwWindow> window_;
  std::unordered_map<uint32_t, interface::Input> inputs_;

  static int to_glfw_keycode(interface::Input::Source source)
  {
    switch (source)
    {
    case (interface::Input::KEY_SPACE): return GLFW_KEY_SPACE;
    case (interface::Input::KEY_APOSTROPHE): return GLFW_KEY_APOSTROPHE;
    case (interface::Input::KEY_COMMA): return GLFW_KEY_COMMA;
    case (interface::Input::KEY_MINUS): return GLFW_KEY_MINUS;
    case (interface::Input::KEY_PERIOD): return GLFW_KEY_PERIOD;
    case (interface::Input::KEY_SLASH): return GLFW_KEY_SLASH;
    case (interface::Input::KEY_0): return GLFW_KEY_0;
    case (interface::Input::KEY_1): return GLFW_KEY_1;
    case (interface::Input::KEY_2): return GLFW_KEY_2;
    case (interface::Input::KEY_3): return GLFW_KEY_3;
    case (interface::Input::KEY_4): return GLFW_KEY_4;
    case (interface::Input::KEY_5): return GLFW_KEY_5;
    case (interface::Input::KEY_6): return GLFW_KEY_6;
    case (interface::Input::KEY_7): return GLFW_KEY_7;
    case (interface::Input::KEY_8): return GLFW_KEY_8;
    case (interface::Input::KEY_9): return GLFW_KEY_9;
    case (interface::Input::KEY_SEMICOLON): return GLFW_KEY_SEMICOLON;
    case (interface::Input::KEY_EQUAL): return GLFW_KEY_EQUAL;
    case (interface::Input::KEY_A): return GLFW_KEY_A;
    case (interface::Input::KEY_B): return GLFW_KEY_B;
    case (interface::Input::KEY_C): return GLFW_KEY_C;
    case (interface::Input::KEY_D): return GLFW_KEY_D;
    case (interface::Input::KEY_E): return GLFW_KEY_E;
    case (interface::Input::KEY_F): return GLFW_KEY_F;
    case (interface::Input::KEY_G): return GLFW_KEY_G;
    case (interface::Input::KEY_H): return GLFW_KEY_H;
    case (interface::Input::KEY_I): return GLFW_KEY_I;
    case (interface::Input::KEY_J): return GLFW_KEY_J;
    case (interface::Input::KEY_K): return GLFW_KEY_K;
    case (interface::Input::KEY_L): return GLFW_KEY_L;
    case (interface::Input::KEY_M): return GLFW_KEY_M;
    case (interface::Input::KEY_N): return GLFW_KEY_N;
    case (interface::Input::KEY_O): return GLFW_KEY_O;
    case (interface::Input::KEY_P): return GLFW_KEY_P;
    case (interface::Input::KEY_Q): return GLFW_KEY_Q;
    case (interface::Input::KEY_R): return GLFW_KEY_R;
    case (interface::Input::KEY_S): return GLFW_KEY_S;
    case (interface::Input::KEY_T): return GLFW_KEY_T;
    case (interface::Input::KEY_U): return GLFW_KEY_U;
    case (interface::Input::KEY_V): return GLFW_KEY_V;
    case (interface::Input::KEY_W): return GLFW_KEY_W;
    case (interface::Input::KEY_X): return GLFW_KEY_X;
    case (interface::Input::KEY_Y): return GLFW_KEY_Y;
    case (interface::Input::KEY_Z): return GLFW_KEY_Z;
    case (interface::Input::KEY_LEFT_BRACKET): return GLFW_KEY_LEFT_BRACKET;
    case (interface::Input::KEY_BACKSLASH): return GLFW_KEY_BACKSLASH;
    case (interface::Input::KEY_RIGHT_BRACKET): return GLFW_KEY_RIGHT_BRACKET;
    case (interface::Input::KEY_GRAVE_ACCENT): return GLFW_KEY_GRAVE_ACCENT;
    case (interface::Input::KEY_WORLD_1): return GLFW_KEY_WORLD_1;
    case (interface::Input::KEY_WORLD_2): return GLFW_KEY_WORLD_2;
    case (interface::Input::KEY_ESCAPE): return GLFW_KEY_ESCAPE;
    case (interface::Input::KEY_ENTER): return GLFW_KEY_ENTER;
    case (interface::Input::KEY_TAB): return GLFW_KEY_TAB;
    case (interface::Input::KEY_BACKSPACE): return GLFW_KEY_BACKSPACE;
    case (interface::Input::KEY_INSERT): return GLFW_KEY_INSERT;
    case (interface::Input::KEY_DELETE): return GLFW_KEY_DELETE;
    case (interface::Input::KEY_RIGHT): return GLFW_KEY_RIGHT;
    case (interface::Input::KEY_LEFT): return GLFW_KEY_LEFT;
    case (interface::Input::KEY_DOWN): return GLFW_KEY_DOWN;
    case (interface::Input::KEY_UP): return GLFW_KEY_UP;
    case (interface::Input::KEY_PAGE_UP): return GLFW_KEY_PAGE_UP;
    case (interface::Input::KEY_PAGE_DOWN): return GLFW_KEY_PAGE_DOWN;
    case (interface::Input::KEY_HOME): return GLFW_KEY_HOME;
    case (interface::Input::KEY_END): return GLFW_KEY_END;
    case (interface::Input::KEY_CAPS_LOCK): return GLFW_KEY_CAPS_LOCK;
    case (interface::Input::KEY_SCROLL_LOCK): return GLFW_KEY_SCROLL_LOCK;
    case (interface::Input::KEY_NUM_LOCK): return GLFW_KEY_NUM_LOCK;
    case (interface::Input::KEY_PRINT_SCREEN): return GLFW_KEY_PRINT_SCREEN;
    case (interface::Input::KEY_PAUSE): return GLFW_KEY_PAUSE;
    case (interface::Input::KEY_F1): return GLFW_KEY_F1;
    case (interface::Input::KEY_F2): return GLFW_KEY_F2;
    case (interface::Input::KEY_F3): return GLFW_KEY_F3;
    case (interface::Input::KEY_F4): return GLFW_KEY_F4;
    case (interface::Input::KEY_F5): return GLFW_KEY_F5;
    case (interface::Input::KEY_F6): return GLFW_KEY_F6;
    case (interface::Input::KEY_F7): return GLFW_KEY_F7;
    case (interface::Input::KEY_F8): return GLFW_KEY_F8;
    case (interface::Input::KEY_F9): return GLFW_KEY_F9;
    case (interface::Input::KEY_F10): return GLFW_KEY_F10;
    case (interface::Input::KEY_F11): return GLFW_KEY_F11;
    case (interface::Input::KEY_F12): return GLFW_KEY_F12;
    case (interface::Input::KEY_F13): return GLFW_KEY_F13;
    case (interface::Input::KEY_F14): return GLFW_KEY_F14;
    case (interface::Input::KEY_F15): return GLFW_KEY_F15;
    case (interface::Input::KEY_F16): return GLFW_KEY_F16;
    case (interface::Input::KEY_F17): return GLFW_KEY_F17;
    case (interface::Input::KEY_F18): return GLFW_KEY_F18;
    case (interface::Input::KEY_F19): return GLFW_KEY_F19;
    case (interface::Input::KEY_F20): return GLFW_KEY_F20;
    case (interface::Input::KEY_F21): return GLFW_KEY_F21;
    case (interface::Input::KEY_F22): return GLFW_KEY_F22;
    case (interface::Input::KEY_F23): return GLFW_KEY_F23;
    case (interface::Input::KEY_F24): return GLFW_KEY_F24;
    case (interface::Input::KEY_F25): return GLFW_KEY_F25;
    case (interface::Input::KEY_KEYPAD_0): return GLFW_KEY_KP_0;
    case (interface::Input::KEY_KEYPAD_1): return GLFW_KEY_KP_1;
    case (interface::Input::KEY_KEYPAD_2): return GLFW_KEY_KP_2;
    case (interface::Input::KEY_KEYPAD_3): return GLFW_KEY_KP_3;
    case (interface::Input::KEY_KEYPAD_4): return GLFW_KEY_KP_4;
    case (interface::Input::KEY_KEYPAD_5): return GLFW_KEY_KP_5;
    case (interface::Input::KEY_KEYPAD_6): return GLFW_KEY_KP_6;
    case (interface::Input::KEY_KEYPAD_7): return GLFW_KEY_KP_7;
    case (interface::Input::KEY_KEYPAD_8): return GLFW_KEY_KP_8;
    case (interface::Input::KEY_KEYPAD_9): return GLFW_KEY_KP_9;
    case (interface::Input::KEY_KEYPAD_DECIMAL): return GLFW_KEY_KP_DECIMAL;
    case (interface::Input::KEY_KEYPAD_DIVIDE): return GLFW_KEY_KP_DIVIDE;
    case (interface::Input::KEY_KEYPAD_MULTIPLY): return GLFW_KEY_KP_MULTIPLY;
    case (interface::Input::KEY_KEYPAD_SUBTRACT): return GLFW_KEY_KP_SUBTRACT;
    case (interface::Input::KEY_KEYPAD_ADD): return GLFW_KEY_KP_ADD;
    case (interface::Input::KEY_KEYPAD_ENTER): return GLFW_KEY_KP_ENTER;
    case (interface::Input::KEY_KEYPAD_EQUAL): return GLFW_KEY_KP_EQUAL;
    case (interface::Input::KEY_LEFT_SHIFT): return GLFW_KEY_LEFT_SHIFT;
    case (interface::Input::KEY_LEFT_CONTROL): return GLFW_KEY_LEFT_CONTROL;
    case (interface::Input::KEY_LEFT_ALT): return GLFW_KEY_LEFT_ALT;
    case (interface::Input::KEY_LEFT_SUPER): return GLFW_KEY_LEFT_SUPER;
    case (interface::Input::KEY_RIGHT_SHIFT): return GLFW_KEY_RIGHT_SHIFT;
    case (interface::Input::KEY_RIGHT_CONTROL): return GLFW_KEY_RIGHT_CONTROL;
    case (interface::Input::KEY_RIGHT_ALT): return GLFW_KEY_RIGHT_ALT;
    case (interface::Input::KEY_RIGHT_SUPER): return GLFW_KEY_RIGHT_SUPER;
    case (interface::Input::KEY_MENU): return GLFW_KEY_MENU;
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

  void register_input(interface::Input input) override
  {
    inputs_.insert({ input.input_code, input });
  }

  uint32_t input_tick(uint32_t input_code) const override
  {
    return inputs_.at(input_code).ticks;
  }

  glm::ivec2 pointer_position() const override
  {
    return glm::ivec2(0);
  }

  glm::ivec2 pointer_delta() const override
  {
    return glm::ivec2(0);
  }

  void update() override
  {
    if (!window_)
      return;

    for (auto& [input_code, input] : inputs_)
    {
      if (window_->glfw_window()->key_pressed(to_glfw_keycode(input.source)))
        input.ticks++;
      else
        input.ticks = 0;
    }
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::GlfwWindowService> Axgl::use_service()
{
  glfw::Window::initialize();

  auto glfw_service = std::make_shared<impl::GlfwWindowService>();
  register_service("window", glfw_service);

  return glfw_service;
}

template<>
std::shared_ptr<impl::GlfwInputService> Axgl::use_service()
{
  auto input_service = std::make_shared<impl::GlfwInputService>();
  register_service("input", input_service);

  return input_service;
}

NAMESPACE_AXGL_END
