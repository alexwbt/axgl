#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "axgl/window.h"
#include "axgl/gameloop.h"
#include "axgl/world/world.h"

class Playground : public axgl::Component
{
  std::shared_ptr<glfw::Window> window_;

  std::shared_ptr<axgl::world::World> world_;

public:
  void initialize()
  {
    spdlog::set_level(spdlog::level::info);

    // initialize glfw
    glfw::Window::initialize();
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      SPDLOG_CRITICAL("Failed to initialize GLAD.");
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_FRONT);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // create window
    window_ = std::make_shared<glfw::Window>(800, 600, "Hello World");
    window_->set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // create world
    world_ = std::make_shared<axgl::world::World>();
    world_->initialize();
  }

  void terminate()
  {
    world_->terminate();

    glfw::Window::terminate();
  }

  void update()
  {
    world_->update();
  }

  void render()
  {
    world_->render();

    glfw::Window::update_all();
  }

  bool alive()
  {
    return !glfw::Window::should_close_all();
  }

};
