#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <ranges>

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace glfw
{

struct EventListener
{
  virtual ~EventListener() = default;
  virtual void on_key_down(int key) { }
  virtual void on_key_up(int key) { }
  virtual void on_mouse_down(int button) { }
  virtual void on_mouse_up(int button) { }
  virtual void on_mouse_move(double x, double y) { }
  virtual void on_scroll(double x, double y) { }
  virtual void on_resize(int width, int height) { }
};

class Window final
{
  inline static bool initialized_ = false;
  inline static bool terminated_ = false;
  inline static std::unordered_map<int, int> window_hints_;
  inline static std::unordered_map<int, std::string> string_window_hints_;
  inline static std::unordered_map<GLFWwindow*, axgl::ptr_t<Window>> windows_;

public:
  static axgl::ptr_t<Window> create(const int width, const int height, const std::string& title)
  {
    glfwDefaultWindowHints();

    for (const auto& [hint, value] : window_hints_)
      glfwWindowHint(hint, value);

    for (const auto& [hint, value] : string_window_hints_)
      glfwWindowHintString(hint, value.c_str());

    // the pointer and Window had to be allocated separately because the Window constructor is private
    const axgl::ptr_t<Window> window(new Window(width, height, title));
    windows_.emplace(window->glfw_window_, window);
    return window;
  }

  static void initialize()
  {
    if (initialized_ || terminated_) return;

    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
    {
      AXGL_LOG_ERROR("Failed to initialize GLFW.");
      return;
    }

    initialized_ = true;
  }

  static void terminate()
  {
    if (!initialized_ || terminated_) return;

    for (const auto& window : windows_ | std::views::values)
      window->destroy();

    glfwTerminate();
    terminated_ = true;
  }

  static void set_hint(int hint, int value) { window_hints_.emplace(hint, value); }

  static void set_hint(int hint, const std::string& value) { string_window_hints_.emplace(hint, value); }

  static void clear_hints()
  {
    window_hints_.clear();
    string_window_hints_.clear();
  }

  static bool should_close_all()
  {
    for (const auto& glfw_window : windows_ | std::views::keys)
      if (!glfwWindowShouldClose(glfw_window)) return false;
    return true;
  }

  static void update_all()
  {
    glfwPollEvents();

    // destroy closed windows
    for (auto it = windows_.begin(); it != windows_.end();)
    {
      if (it->second->destroyed_ || glfwWindowShouldClose(it->first))
      {
        it->second->destroy();
        it = windows_.erase(it);
      }
      else ++it;
    }
  }

private:
  static void glfw_error_callback(int error, const char* description)
  {
    AXGL_LOG_ERROR("GLFW Error {}: {}", error, description);
  }

  static void key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods)
  {
    const auto listener = get_window_event_listener(glfw_window);
    if (!listener) return;

    switch (action)
    {
    case GLFW_PRESS: listener->on_key_down(key); break;
    case GLFW_RELEASE: listener->on_key_up(key); break;
    default:;
    }
  }

  static void cursor_pos_callback(GLFWwindow* glfw_window, double x, double y)
  {
    if (const auto listener = get_window_event_listener(glfw_window)) listener->on_mouse_move(x, y);
  }

  static void scroll_callback(GLFWwindow* glfw_window, double x, double y)
  {
    const auto window = get_window(glfw_window);
    window->scroll_x_ += x;
    window->scroll_y_ += y;

    if (const auto listener = get_window_event_listener(glfw_window)) listener->on_scroll(x, y);
  }

  static void mouse_button_callback(GLFWwindow* glfw_window, int button, int action, int mods)
  {
    const auto listener = get_window_event_listener(glfw_window);
    if (!listener) return;

    switch (action)
    {
    case GLFW_PRESS: listener->on_mouse_down(button); break;
    case GLFW_RELEASE: listener->on_mouse_up(button); break;
    default:;
    }
  }

  static void frame_buffer_size_callback(GLFWwindow* glfw_window, int width, int height)
  {
    if (const auto listener = get_window_event_listener(glfw_window)) listener->on_resize(width, height);
  }

  static axgl::ptr_t<Window> get_window(GLFWwindow* glfw_window)
  {
    try
    {
      return windows_.at(glfw_window);
    }
    catch ([[maybe_unused]] const std::out_of_range& e)
    {
      AXGL_LOG_DEBUG("Tried to get GLFW window that does not exists. ({})", e.what());
      return nullptr;
    }
  }

  static axgl::ptr_t<EventListener> get_window_event_listener(GLFWwindow* glfw_window)
  {
    const auto window = get_window(glfw_window);
    return window ? window->event_listener_ : nullptr;
  }

  GLFWwindow* glfw_window_;
  axgl::ptr_t<EventListener> event_listener_;
  std::unordered_map<int, GLFWcursor*> cursors_;
  double scroll_x_ = 0.0;
  double scroll_y_ = 0.0;
  bool destroyed_ = false;

  Window(int width, int height, const std::string& title) noexcept
  {
    if (!initialized_ || terminated_)
    {
      AXGL_LOG_ERROR("Failed to create window, GLFW not initialized.");
      return;
    }

    glfw_window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!glfw_window_)
    {
      AXGL_LOG_ERROR("Failed to create window: {}", title);
      return;
    }

    glfwSetKeyCallback(glfw_window_, key_callback);
    glfwSetScrollCallback(glfw_window_, scroll_callback);
    glfwSetCursorPosCallback(glfw_window_, cursor_pos_callback);
    glfwSetMouseButtonCallback(glfw_window_, mouse_button_callback);
    glfwSetFramebufferSizeCallback(glfw_window_, frame_buffer_size_callback);
  }

public:
  ~Window() { destroy(); }

  void set_title(const std::string& title) const { glfwSetWindowTitle(glfw_window_, title.c_str()); }
  void set_size(int width, int height) const { glfwSetWindowSize(glfw_window_, width, height); }
  void set_position(int x, int y) const { glfwSetWindowPos(glfw_window_, x, y); }
  void set_input_mode(int mode, int value) const { glfwSetInputMode(glfw_window_, mode, value); }
  void set_event_listener(axgl::ptr_t<EventListener> event_listener) { event_listener_ = std::move(event_listener); }

  [[nodiscard]] GLFWwindow* get_glfw_window() const { return glfw_window_; }
  [[nodiscard]] bool is_destroyed() const { return destroyed_; }
  [[nodiscard]] bool key_down(int key) const { return glfwGetKey(glfw_window_, key) == GLFW_PRESS; }
  [[nodiscard]] bool mouse_down(int button) const { return glfwGetMouseButton(glfw_window_, button) == GLFW_PRESS; }
  [[nodiscard]] glm::vec2 get_mouse_pos() const
  {
    double x, y;
    glfwGetCursorPos(glfw_window_, &x, &y);
    return {x, y};
  }
  [[nodiscard]] glm::vec2 get_scroll() const { return {scroll_x_, scroll_y_}; }

  void reset_scroll()
  {
    scroll_x_ = 0;
    scroll_y_ = 0;
  }

  void use() const { glfwMakeContextCurrent(glfw_window_); }
  void swap_buffers() const { glfwSwapBuffers(glfw_window_); }

  [[nodiscard]] glm::ivec2 get_size() const
  {
    int width, height;
    glfwGetFramebufferSize(glfw_window_, &width, &height);
    return {width, height};
  }

  void use_standard_cursor(int shape)
  {
    if (!cursors_.contains(shape)) cursors_[shape] = glfwCreateStandardCursor(shape);

    glfwSetCursor(glfw_window_, cursors_[shape]);
  }

private:
  void destroy()
  {
    if (!destroyed_)
    {
      glfwDestroyWindow(glfw_window_);
      destroyed_ = true;
    }
  }
};

} // namespace glfw
