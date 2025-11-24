#pragma once

#include <string>
#include <memory>
#include <cstdint>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>

#include <glm/glm.hpp>

NAMESPACE_AXGL_INTERFACE

class Window
{
public:
  virtual ~Window() = default;
  virtual void set_title(const std::string& title) = 0;
  virtual void set_position(uint32_t x, uint32_t y) = 0;
  virtual void set_size(uint32_t width, uint32_t height) = 0;
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

class WindowService : virtual public Service
{
public:
  virtual std::shared_ptr<Window> create_window() = 0;
};

NAMESPACE_AXGL_INTERFACE_END
