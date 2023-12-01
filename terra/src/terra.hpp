#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "axgl/gameloop.h"
#include "axgl/window.h"

#include "axgl/world/world.h"

class Terra : public axgl::Component
{
  glfw::Window window_; // FIXME: initialized before GLFW
  axgl::world::World world_;

public:
  Terra() : window_(800, 600, "Terra") {}

  void initialize() override
  {
    glfw::Window::initialize();
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      SPDLOG_CRITICAL("Failed to initialize GLAD.");

    world_.initialize();
  }

  void terminate() override
  {
    world_.terminate();
    glfw::Window::terminate();
  }

  void update() override
  {
    world_.update();
  }

  void render() override
  {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // world_.render();
    window_.swap_buffers();

    glfw::Window::update_all();
  }

  bool alive() override
  {
    return false;
  }
};
