#pragma once

#include <unordered_map>

#include <axgl/axgl.hpp>
#include <axgl/namespace.hpp>
#include <axgl/interface/input.hpp>
#include <axgl/interface/window.hpp>
#include <axgl/impl/glfw/glfw_window.hpp>

NAMESPACE_AXGL_IMPL

class GlfwWindow : public interface::Window
{
private:
  std::shared_ptr<glfw::Window> window_;

public:
  GlfwWindow(std::shared_ptr<glfw::Window> window)
    : window_(std::move(window))
  {}

  void set_title(const std::string& title) override
  {
    if (!window_->is_destroyed())
      window_->set_title(title);
  }

  void set_position(uint32_t x, uint32_t y) override
  {
    if (!window_->is_destroyed())
      window_->set_position(x, y);
  }

  void set_size(uint32_t width, uint32_t height) override
  {
    if (!window_->is_destroyed())
      window_->set_size(width, height);
  }

  void maximize() override {}

  void hide() override {}

  void show() override {}

  void close() override {}

  bool ready() const override
  {
    return !window_->is_destroyed();
  }

  void use() const override
  {
    if (!window_->is_destroyed())
      window_->use();
  }

  void swap_buffers() const override
  {
    if (!window_->is_destroyed())
      window_->swap_buffers();
  }

  Size get_size() const override
  {
    auto size = window_->get_size();
    return {
      static_cast<uint32_t>(std::max(0, size.width)),
      static_cast<uint32_t>(std::max(0, size.height))
    };
  }

public:
  auto glfw_window() const
  {
    return window_;
  }
};

class GlfwWindowService : public interface::WindowService
{
public:
  void terminate() override
  {
    glfw::Window::terminate();
  }

  void render() override
  {
    glfw::Window::update_all();
  }

  bool running() override
  {
    return !glfw::Window::should_close_all();
  }

  bool keep_alive() override
  {
    return running();
  }

  std::shared_ptr<interface::Window> create_window() override
  {
    return std::make_shared<GlfwWindow>(glfw::Window::create(800, 600, ""));
  }
};

class GlfwInputService : public interface::InputService
{
private:
  std::unordered_map<uint32_t, interface::Input> inputs_;

public:
  void register_input(interface::Input input) override
  {
    inputs_.insert({ input.input_code, input });
  }

  uint32_t input_tick(uint32_t input_code) const override
  {
    return inputs_.at(input_code).ticks;
  }

  glm::ivec2 pointer_position() const override
  {
    return glm::ivec2(0);
  }

  glm::ivec2 pointer_delta() const override
  {
    return glm::ivec2(0);
  }

  void update() override
  {
    for (const auto& [input_code, input] : inputs_)
    {

    }
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::GlfwWindowService> Axgl::use_service()
{
  glfw::Window::initialize();

  auto glfw_service = std::make_shared<impl::GlfwWindowService>();
  register_service("window", glfw_service);

  return glfw_service;
}

template<>
std::shared_ptr<impl::GlfwInputService> Axgl::use_service()
{
  auto input_service = std::make_shared<impl::GlfwInputService>();
  register_service("input", input_service);

  return input_service;
}

NAMESPACE_AXGL_END
