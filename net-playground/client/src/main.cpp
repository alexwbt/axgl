#include <glad/glad.h>
#include <axgl/gameloop.h>
#include <axgl/glfw/window.h>
#include <spdlog/spdlog.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class Playground :
  public axgl::Component,
  public axgl::glfw::Window::EventListener
{
  axgl::glfw::Window* window_;

public:
  void initialize() override
  {
    // initialize glfw
    axgl::glfw::Window::initialize();
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create window
    window_ = new axgl::glfw::Window(800, 600, "Playground");
    // window_->set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    window_->set_event_listener(this);
    window_->use();

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      SPDLOG_CRITICAL("Failed to initialize GLAD.");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window_->get_glfw_window(), true);
    ImGui_ImplOpenGL3_Init();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
  }

  void render() override
  {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    window_->swap_buffers();
    axgl::glfw::Window::update_all();
  }

  bool alive() override
  {
    return !axgl::glfw::Window::should_close_all();
  }

  void terminate() override
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    axgl::glfw::Window::terminate();
  }

  ~Playground()
  {
    delete window_;
  }
};


int main()
{
  Playground playground;

  axgl::Gameloop gameloop;
  gameloop.add_component(&playground);
  gameloop.run();
}
