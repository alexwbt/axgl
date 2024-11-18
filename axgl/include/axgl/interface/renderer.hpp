#pragma once

#include <memory>

#include "axgl/namespace.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/interface/window.hpp"

NAMESPACE_AXGL_INTERFACE

class Renderer
{
public:
  virtual ~Renderer() {}
  virtual void before_render() = 0;
  virtual void after_render() = 0;
  virtual void set_window(std::shared_ptr<Window> window) = 0;
};

class RendererService : public Service
{
public:
  virtual std::shared_ptr<Renderer> create_renderer() = 0;
};

NAMESPACE_AXGL_INTERFACE_END
