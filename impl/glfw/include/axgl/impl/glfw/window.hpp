#pragma once

#include <cstdint>
#include <memory>

#include <axgl/interface/window.hpp>

#include <glfw/window.hpp>

namespace axgl::impl::glfw
{

class Window : public axgl::Window
{
  std::shared_ptr<::glfw::Window> window_;

public:
  explicit Window(std::shared_ptr<::glfw::Window> window) : window_(std::move(window)) { }

  void set_title(const std::string& title) override
  {
    if (!window_->is_destroyed())
      window_->set_title(title);
  }

  void set_position(const std::uint32_t x, const std::uint32_t y) override
  {
    if (!window_->is_destroyed())
      window_->set_position(x, y);
  }

  void set_size(const std::uint32_t width, const std::uint32_t height) override
  {
    if (!window_->is_destroyed())
      window_->set_size(width, height);
  }

  void maximize() override { }

  void hide() override { }

  void show() override { }

  void close() override { }

  [[nodiscard]] bool ready() const override { return !window_->is_destroyed(); }

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

  [[nodiscard]] glm::ivec2 get_size() const override { return window_->get_size(); }

  auto glfw_window() const { return window_; }
};

} // namespace axgl::impl::glfw
