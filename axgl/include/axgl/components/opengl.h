#pragma once

#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>

#include "axgl/gameloop.h"
#include "axgl/glfw/window.h"
#include "axgl/components/window_event_adapter.h"


NAMESPACE_AXGL

class OpenglWindow : public ComponentParent
{
  std::shared_ptr<glfw::Window> window_;
  std::shared_ptr<WindowEventAdapter> window_event_adapter_;

  int initial_width_, initial_height_;
  const std::string initial_title_;

public:
  OpenglWindow(int width, int height, const std::string& title) :
    initial_width_(width), initial_height_(height), initial_title_(title) {}

  OpenglWindow() : OpenglWindow(800, 600, "OpenGL") {}

  virtual ~OpenglWindow() {}

  auto window() {
    return std::weak_ptr<glfw::Window>(window_);
  }

  void set_event_adapter(std::shared_ptr<WindowEventAdapter> event_adapter)
  {
    window_event_adapter_ = std::move(event_adapter);
  }

  void initialize(ComponentContext& context) override
  {
    // initialize glfw
    glfw::Window::initialize();
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create window
    window_ = glfw::Window::create(initial_width_, initial_height_, initial_title_);
    window_->use();

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      SPDLOG_CRITICAL("Failed to initialize GLAD.");

    // set event adapter if provided
    if (window_event_adapter_)
    {
      add_component(window_event_adapter_);
      window_->set_event_listener(window_event_adapter_);
    }

    ComponentParent::initialize(context);
  }

  void render(ComponentContext& context) override
  {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ComponentParent::render(context);

    window_->swap_buffers();
    glfw::Window::update_all();
  }

  bool alive(ComponentContext& context) override
  {
    return !glfw::Window::should_close_all();
  }

  void terminate(ComponentContext& context) override
  {
    ComponentParent::terminate(context);
    glfw::Window::terminate();
  }
};

class OpenglImgui : public ComponentParent
{
  std::shared_ptr<OpenglWindow> opengl_window_;

public:
  OpenglImgui(std::shared_ptr<OpenglWindow> opengl_window) :
    opengl_window_(std::move(opengl_window)) {}


  void initialize(ComponentContext& context) override
  {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Setup Platform/Renderer backends
    {
      auto window = opengl_window_->window().lock();
      ImGui_ImplGlfw_InitForOpenGL(window->get_glfw_window(), true);
    }
    ImGui_ImplOpenGL3_Init();

    ComponentParent::initialize(context);
  }

  void render(ComponentContext& context) override
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ComponentParent::render(context);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
};

NAMESPACE_AXGL_END
