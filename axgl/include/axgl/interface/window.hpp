#pragma once

#include <cstdint>
#include <string>

#include <glm/glm.hpp>

namespace axgl
{

class Window
{
public:
  virtual ~Window() = default;
  virtual void set_title(const std::string& title) = 0;
  virtual void set_position(std::uint32_t x, std::uint32_t y) = 0;
  virtual void set_size(std::uint32_t width, std::uint32_t height) = 0;
  virtual void maximize() = 0;
  virtual void hide() = 0;
  virtual void show() = 0;
  virtual void close() = 0;
  virtual bool ready() const = 0;
  // render
  virtual void use() const = 0;
  virtual void swap_buffers() const = 0;
  virtual glm::ivec2 get_size() const = 0;
};

} // namespace axgl
