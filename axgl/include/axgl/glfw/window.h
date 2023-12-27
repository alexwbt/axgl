#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include <GLFW/glfw3.h>

#include "axgl/namespace.h"

NAMESPACE_GLFW

class Window final
{
public:
  struct EventListener
  {
    virtual ~EventListener() {}
    virtual void on_key_down(int key) {}
    virtual void on_key_up(int key) {}
    virtual void on_mouse_down(int button) {}
    virtual void on_mouse_up(int button) {}
    virtual void on_mouse_move(double x, double y) {}
    virtual void on_resize(int width, int height) {}
  };

private:
  static bool initialized_;
  static bool terminated_;
  static std::unordered_map<GLFWwindow*, Window*> windows_;

public:
  static void initialize();
  static void terminate();

  static bool should_close_all();
  static void update_all();

private:
  static void glfw_error_callback(int error, const char* description);

  static void key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
  static void cursor_pos_callback(GLFWwindow* glfw_window, double x, double y);
  static void mouse_button_callback(GLFWwindow* glfw_window, int button, int action, int mods);
  static void frame_buffer_size_callback(GLFWwindow* glfw_window, int width, int height);

  static Window* get_window(GLFWwindow* glfw_window);
  static EventListener* get_window_event_listener(GLFWwindow* glfw_window);

private:
  GLFWwindow* glfw_window_;

  EventListener* event_listener_;

  bool destroyed_ = false;

public:
  Window(int width, int height, const std::string& title);
  ~Window();

public:
  void set_title(const std::string& title);
  void set_input_mode(int mode, int value);
  // does not take ownership of event listener
  void set_event_listener(EventListener* event_listener);

  GLFWwindow* get_glfw_window() const;
  bool is_destroyed() const { return destroyed_; }
  bool key_pressed(int key) const;

  void use() const;
  void swap_buffers() const;

private:
  void destroy();
};

NAMESPACE_GLFW_END
