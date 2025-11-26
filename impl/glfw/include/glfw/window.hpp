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
  virtual void on_resize(int width, int height) { }
};

class Window final
{
  inline static bool initialized_ = false;
  inline static bool terminated_ = false;
  inline static std::unordered_map<GLFWwindow*, std::shared_ptr<Window>> windows_;

public:
  static std::shared_ptr<Window> create(const int width, const int height, const std::string& title)
  {
    std::shared_ptr<Window> window(new Window(width, height, title));
    windows_.insert({window->glfw_window_, window});
    return window;
  }

  static void initialize()
  {
    if (initialized_ || terminated_)
      return;

    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
    {
      SPDLOG_CRITICAL("Failed to initialize GLFW.");
      return;
    }

    initialized_ = true;
  }

  static void terminate()
  {
    if (!initialized_ || terminated_)
      return;

    for (const auto& window : windows_ | std::views::values)
      window->destroy();

    glfwTerminate();
    terminated_ = true;
  }

  static void glfw_window_hint(int hint, int value) { glfwWindowHint(hint, value); }

  static bool should_close_all()
  {
    for (const auto& glfw_window : windows_ | std::views::keys)
      if (!glfwWindowShouldClose(glfw_window))
        return false;
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
      else
        ++it;
    }
  }

private:
  static void glfw_error_callback(int error, const char* description)
  {
    SPDLOG_ERROR("GLFW Error {}: {}", error, description);
  }

  static void key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods)
  {
    const auto listener = get_window_event_listener(glfw_window);
    if (!listener)
      return;

    switch (action)
    {
    case GLFW_PRESS: listener->on_key_down(key); break;
    case GLFW_RELEASE: listener->on_key_up(key); break;
    default:;
    }
  }

  static void cursor_pos_callback(GLFWwindow* glfw_window, double x, double y)
  {
    if (const auto listener = get_window_event_listener(glfw_window))
      listener->on_mouse_move(x, y);
  }

  static void mouse_button_callback(GLFWwindow* glfw_window, int button, int action, int mods)
  {
    const auto listener = get_window_event_listener(glfw_window);
    if (!listener)
      return;

    switch (action)
    {
    case GLFW_PRESS: listener->on_mouse_down(button); break;
    case GLFW_RELEASE: listener->on_mouse_up(button); break;
    default:;
    }
  }

  static void frame_buffer_size_callback(GLFWwindow* glfw_window, int width, int height)
  {
    if (const auto listener = get_window_event_listener(glfw_window))
      listener->on_resize(width, height);
  }

  static std::shared_ptr<Window> get_window(GLFWwindow* glfw_window)
  {
    try
    {
      return windows_.at(glfw_window);
    }
    catch ([[maybe_unused]] const std::out_of_range& e)
    {
      SPDLOG_DEBUG("Tried to get GLFW window that does not exists. ({})", e.what());
      return nullptr;
    }
  }

  static std::shared_ptr<EventListener> get_window_event_listener(GLFWwindow* glfw_window)
  {
    auto window = get_window(glfw_window);
    if (!window)
      return nullptr;

    return window->event_listener_;
  }

  GLFWwindow* glfw_window_;

  std::shared_ptr<EventListener> event_listener_;

  bool destroyed_ = false;

  Window(int width, int height, const std::string& title)
  {
    if (!initialized_ || terminated_)
    {
      SPDLOG_CRITICAL("Failed to create window, GLFW not initialized.");
      return;
    }

    glfw_window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!glfw_window_)
    {
      SPDLOG_CRITICAL("Failed to create window: {}", title);
      return;
    }

    glfwSetKeyCallback(glfw_window_, key_callback);
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

  void set_event_listener(std::shared_ptr<EventListener> event_listener)
  {
    event_listener_ = std::move(event_listener);
  }

  [[nodiscard]] GLFWwindow* get_glfw_window() const { return glfw_window_; }
  [[nodiscard]] bool is_destroyed() const { return destroyed_; }
  [[nodiscard]] bool key_pressed(int key) const { return glfwGetKey(glfw_window_, key) == GLFW_PRESS; }
  [[nodiscard]] glm::ivec2 get_mouse_pos() const
  {
    double xpos, ypos;
    glfwGetCursorPos(glfw_window_, &xpos, &ypos);
    return {xpos, ypos};
  }

  void use() const { glfwMakeContextCurrent(glfw_window_); }
  void swap_buffers() const { glfwSwapBuffers(glfw_window_); }

  [[nodiscard]] glm::ivec2 get_size() const
  {
    int width, height;
    glfwGetFramebufferSize(glfw_window_, &width, &height);
    return {width, height};
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
