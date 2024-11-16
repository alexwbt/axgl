#pragma once

#include <stdint.h>
#include <string>
#include <memory>

#include "axgl/namespace.hpp"
#include "axgl/interface/input.hpp"
#include "axgl/interface/renderer.hpp"
#include "axgl/interface/service.hpp"

NAMESPACE_AXGL_INTERFACE

class Window
{
public:
  virtual ~Window() {}
  virtual void set_title(const std::string& title) = 0;
  virtual void set_position(uint32_t x, uint32_t y) = 0;
  virtual void set_size(uint32_t width, uint32_t height) = 0;
  virtual void maximize() = 0;
  virtual void hide() = 0;
  virtual void show() = 0;
  virtual void close() = 0;

  virtual std::shared_ptr<Renderer> get_renderer() = 0;
  virtual std::shared_ptr<Input> get_input() = 0;
};

class WindowService : public Service
{
public:
  virtual std::shared_ptr<Window> create_window() = 0;
};

NAMESPACE_AXGL_INTERFACE_END
