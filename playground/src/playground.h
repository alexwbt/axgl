#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "axgl/gameloop.h"
#include "axgl/glfw/window.h"
#include "axgl/world/world.h"
#include "axgl/world/camera.h"
#include "axgl/world/light.h"
#include "axgl/world/render_context.h"
#include "axgl/opengl/entity/skybox.h"
#include "axgl/opengl/shader/phong-shader.h"

#include "resource.h"
#include "cube.h"
#include "backpack.h"
#include "chunks.h"

class Playground : public axgl::Component,
  public axgl::glfw::Window::EventListener,
  public std::enable_shared_from_this<Playground>
{
  std::shared_ptr<axgl::glfw::Window> window_;

  std::shared_ptr<axgl::world::World> world_;

  std::shared_ptr<axgl::world::Camera> camera_;
  axgl::world::CameraController camera_controller_;

  axgl::world::RenderContext render_context_;

  Playground() {}

public:
  static auto make_shared()
  {
    return std::shared_ptr<Playground>(new Playground());
  }

  void initialize(axgl::ComponentContext& context) override
  {
    spdlog::set_level(spdlog::level::info);

    // initialize glfw
    axgl::glfw::Window::initialize();
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create window
    window_ = axgl::glfw::Window::create(800, 600, "Hello World");
    window_->set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    window_->set_event_listener(shared_from_this());
    window_->use();
    render_context_.view_width = 800;
    render_context_.view_height = 600;

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      SPDLOG_CRITICAL("Failed to initialize GLAD.");

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // load resources
    load_shaders();
    load_textures();

    // entities
    auto skybox = std::make_shared<axgl::opengl::entity::Skybox>(
      get_shader<axgl::opengl::shader::SkyboxShader>(ShaderID::kSkybox),
      axgl::opengl::load_texture("skybox/void")
    );
    auto cube = std::make_shared<Cube>();
    auto backpack = std::make_shared<Backpack>();
    auto chunk = std::make_shared<Chunk>();

    // create world
    world_ = std::make_shared<axgl::world::World>();
    world_->push_entity(skybox);
    world_->push_entity(cube);
    world_->push_entity(backpack);
    world_->push_entity(chunk);
    world_->initialize();

    // initialize camera
    camera_ = std::make_shared<axgl::world::Camera>();
    camera_->position = { 0.0f, 0.0f, -5.0f };
    camera_->yaw = 90.0f;
    camera_->fov = 100.0f;
    camera_->update_transform();
    camera_controller_.set_camera(camera_);
    render_context_.camera = camera_;
  }

  void terminate(axgl::ComponentContext& context) override
  {
    world_->terminate();

    axgl::glfw::Window::terminate();
  }

  void update(axgl::ComponentContext& context) override
  {
    camera_controller_.move({
        window_->key_pressed(GLFW_KEY_W),
        window_->key_pressed(GLFW_KEY_S),
        window_->key_pressed(GLFW_KEY_A),
        window_->key_pressed(GLFW_KEY_D),
        window_->key_pressed(GLFW_KEY_SPACE),
        window_->key_pressed(GLFW_KEY_LEFT_SHIFT) });

    world_->update();
  }

  void render(axgl::ComponentContext& context) override
  {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_context_.projection = glm::perspective(
      glm::radians(camera_->fov),
      static_cast<float>(render_context_.view_width)
      / static_cast<float>(render_context_.view_height),
      0.1f, 1000.0f);
    render_context_.pv = render_context_.projection * camera_->view_matrix();
    render_context_.view = camera_->view_matrix();

    world_->render(render_context_);

    window_->swap_buffers();

    axgl::glfw::Window::update_all();
  }

  bool alive(axgl::ComponentContext& context) override
  {
    return !axgl::glfw::Window::should_close_all();
  }

  void on_mouse_move(double x, double y) override
  {
    camera_controller_.pivot(
      static_cast<float>(x),
      static_cast<float>(y)
    );
  }

  void on_resize(int width, int height) override
  {
    if (width != render_context_.view_width || height != render_context_.view_height)
    {
      render_context_.view_width = static_cast<uint32_t>(width);
      render_context_.view_height = static_cast<uint32_t>(height);
      glViewport(0, 0, width, height);
    }
  }

};
