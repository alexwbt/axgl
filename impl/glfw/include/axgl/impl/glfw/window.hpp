#pragma once

#include <memory>

#include <axgl/common.hpp>
#include <axgl/interface/window.hpp>
#include <axgl/impl/service.hpp>

#include <glfw/window.hpp>

namespace axgl::impl
{

class GlfwWindow : public interface::Window
{
  std::shared_ptr<glfw::Window> window_;

public:
  explicit GlfwWindow(std::shared_ptr<glfw::Window> window) : window_(std::move(window)) { }

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

  void set_size(const uint32_t width, const uint32_t height) override
  {
    if (!window_->is_destroyed())
      window_->set_size(width, height);
  }

  void maximize() override { }

  void hide() override { }

  void show() override { }

  void close() override { }

  bool ready() const override { return !window_->is_destroyed(); }

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

  glm::ivec2 get_size() const override { return window_->get_size(); }

  auto glfw_window() const { return window_; }
};

class GlfwWindowService final : public interface::WindowService, public ServiceBase
{
public:
  void terminate() override { glfw::Window::terminate(); }

  void render() override { glfw::Window::update_all(); }

  bool running() override { return !glfw::Window::should_close_all(); }

  bool keep_alive() override { return running(); }

  std::shared_ptr<interface::Window> create_window() override
  {
    return std::make_shared<GlfwWindow>(glfw::Window::create(800, 600, ""));
  }

  void set_window_hint(int hint, int value) const { glfw::Window::glfw_window_hint(hint, value); }
};

} // namespace axgl::impl

