#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

class GlContext
{
  GLFWwindow* window_ = nullptr;
  bool context_available_ = false;

public:
  GlContext()
  {
    if (glfwInit() == GLFW_FALSE) return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    window_ = glfwCreateWindow(64, 64, "shader_compile_test", nullptr, nullptr);
    if (!window_) return;

    glfwMakeContextCurrent(window_);
    context_available_
      = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))
      != 0;
  }

  ~GlContext()
  {
    if (window_)
    {
      glfwMakeContextCurrent(nullptr);
      glfwDestroyWindow(window_);
    }
    glfwTerminate();
  }

  GlContext(const GlContext&) = delete;
  GlContext& operator=(const GlContext&) = delete;

  [[nodiscard]] bool available() const { return context_available_; }
};
