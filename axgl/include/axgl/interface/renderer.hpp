#pragma once

#include "axgl/namespace.hpp"

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

NAMESPACE_AXGL_INTERFACE_END
