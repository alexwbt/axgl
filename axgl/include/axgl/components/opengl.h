#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "axgl/gameloop.h"
#include "axgl/glfw/window.h"


NAMESPACE_AXGL

class OpenglWindow : public ComponentParent
{
  glfw::Window* window_;

  int initial_width_, initial_height_;
  const std::string initial_title_;

public:
  OpenglWindow(int width, int height, const std::string& title) :
    initial_width_(width), initial_height_(height), initial_title_(title) {}

  OpenglWindow() : OpenglWindow(800, 600, "OpenGL") {}

  virtual ~OpenglWindow()
  {
    delete window_;
  }

  glfw::Window* window() { return window_; }

  void initialize() override
  {
    // initialize glfw
    axgl::glfw::Window::initialize();
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create window
    window_ = new axgl::glfw::Window(initial_width_, initial_height_, initial_title_);
    window_->use();

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      SPDLOG_CRITICAL("Failed to initialize GLAD.");

    ComponentParent::initialize();
  }

  void render() override
  {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ComponentParent::render();

    window_->swap_buffers();
    axgl::glfw::Window::update_all();
  }

  bool alive() override
  {
    return !axgl::glfw::Window::should_close_all();
  }

  void terminate() override
  {
    ComponentParent::terminate();
    axgl::glfw::Window::terminate();
  }
};

class OpenglImgui : public ComponentParent
{
  axgl::OpenglWindow& opengl_window_;

public:
  OpenglImgui(axgl::OpenglWindow& opengl_window) :
    opengl_window_(opengl_window) {}


  void initialize() override
  {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(opengl_window_.window()->get_glfw_window(), true);
    ImGui_ImplOpenGL3_Init();

    ComponentParent::initialize();
  }

  void render() override
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ComponentParent::render();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
};

NAMESPACE_AXGL_END
