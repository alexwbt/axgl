#pragma once

#include "axgl/namespace.hpp"
#include "axgl/interface/service.hpp"

NAMESPACE_AXGL_INTERFACE

class Renderable
{
public:
  virtual void render() = 0;
};

class Renderer
{
public:
  virtual ~Renderer() {}
  virtual void render(const Renderable& renderable) = 0;
};

class RenderService : public Service
{
public:
  virtual std::shared_ptr<Renderer> get_renderer() = 0;
};

NAMESPACE_AXGL_INTERFACE_END
