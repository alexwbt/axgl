#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

namespace glfw
{

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

  class Window final
  {
  private:
    inline static bool initialized_ = false;
    inline static bool terminated_ = false;
    inline static std::unordered_map<GLFWwindow*, std::shared_ptr<Window>> windows_;

  public:
    static std::shared_ptr<Window> create(int width, int height, const std::string& title)
    {
      std::shared_ptr<Window> window(new Window(width, height, title));
      windows_.insert({ window->glfw_window_, window });
      return window;
    }

    inline static void initialize()
    {
      if (initialized_ || terminated_)
        return;

      glfwSetErrorCallback(Window::glfw_error_callback);

      if (!glfwInit())
      {
        SPDLOG_CRITICAL("Failed to initialize GLFW.");
        return;
      }

      initialized_ = true;
    }

    inline static void terminate()
    {
      if (!initialized_ || terminated_)
        return;

      for (const auto& [glfw_window, window] : windows_)
        window->destroy();

      glfwTerminate();
      terminated_ = true;
    }

    inline static bool should_close_all()
    {
      for (const auto& [glfw_window, window] : windows_)
        if (!glfwWindowShouldClose(glfw_window))
          return false;
      return true;
    }

    inline static void update_all()
    {
      glfwPollEvents();

      // destroy closed windows
      for (auto it = windows_.begin(); it != windows_.end();)
      {
        const auto& glfw_window = it->first;
        const auto& window = it->second;
        if (window->destroyed_ || glfwWindowShouldClose(glfw_window))
        {
          it->second->destroy();
          it = windows_.erase(it);
        }
        else
          ++it;
      }
    }

  private:
    inline static void glfw_error_callback(int error, const char* description)
    {
      SPDLOG_ERROR("GLFW Error {}: {}", error, description);
    }

    inline static void key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods)
    {
      auto listener = get_window_event_listener(glfw_window);
      if (!listener) return;

      switch (action)
      {
      case GLFW_PRESS:    listener->on_key_down(key); break;
      case GLFW_RELEASE:  listener->on_key_up(key); break;
      }
    }

    inline static void cursor_pos_callback(GLFWwindow* glfw_window, double x, double y)
    {
      auto listener = get_window_event_listener(glfw_window);
      if (listener)
        listener->on_mouse_move(x, y);
    }

    inline static void mouse_button_callback(GLFWwindow* glfw_window, int button, int action, int mods)
    {
      auto listener = get_window_event_listener(glfw_window);
      if (!listener) return;

      switch (action)
      {
      case GLFW_PRESS:    listener->on_mouse_down(button); break;
      case GLFW_RELEASE:  listener->on_mouse_up(button); break;
      }
    }

    inline static void frame_buffer_size_callback(GLFWwindow* glfw_window, int width, int height)
    {
      auto listener = get_window_event_listener(glfw_window);
      if (listener)
        listener->on_resize(width, height);
    }

    inline static std::shared_ptr<Window> get_window(GLFWwindow* glfw_window)
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

    inline static std::shared_ptr<EventListener> get_window_event_listener(GLFWwindow* glfw_window)
    {
      auto window = get_window(glfw_window);
      if (!window) return nullptr;

      return window->event_listener_;
    }

  private:
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

      glfwSetKeyCallback(glfw_window_, Window::key_callback);
      glfwSetCursorPosCallback(glfw_window_, Window::cursor_pos_callback);
      glfwSetMouseButtonCallback(glfw_window_, Window::mouse_button_callback);
      glfwSetFramebufferSizeCallback(glfw_window_, Window::frame_buffer_size_callback);
    }

  public:
    ~Window()
    {
      destroy();
    }

    void set_title(const std::string& title)
    {
      glfwSetWindowTitle(glfw_window_, title.c_str());
    }

    void set_size(int width, int height)
    {
      glfwSetWindowSize(glfw_window_, width, height);
    }

    void set_position(int x, int y)
    {
      glfwSetWindowPos(glfw_window_, x, y);
    }

    void set_input_mode(int mode, int value)
    {
      glfwSetInputMode(glfw_window_, mode, value);
    }

    void set_event_listener(std::shared_ptr<EventListener> event_listener)
    {
      event_listener_ = std::move(event_listener);
    }

    GLFWwindow* get_glfw_window() const { return glfw_window_; }
    bool is_destroyed() const { return destroyed_; }
    bool key_pressed(int key) const { return glfwGetKey(glfw_window_, key) == GLFW_PRESS; }

    void use() const { glfwMakeContextCurrent(glfw_window_); }
    void swap_buffers() const { glfwSwapBuffers(glfw_window_); }

    glm::ivec2 get_size() const
    {
      int width, height;
      glfwGetFramebufferSize(glfw_window_, &width, &height);
      return { width, height };
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

}
