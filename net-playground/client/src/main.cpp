#include <glad/glad.h>
#include <axgl/gameloop.h>
#include <axgl/glfw/window.h>
#include <spdlog/spdlog.h>


class Playground : public axgl::Component,
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
  }

  void render() override
  {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    window_->swap_buffers();
    axgl::glfw::Window::update_all();
  }

  bool alive() override
  {
    return !axgl::glfw::Window::should_close_all();
  }

  void terminate() override
  {
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
