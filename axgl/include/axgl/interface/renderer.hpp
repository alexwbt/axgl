#pragma once

#include <format>
#include <stdexcept>

#include "axgl/namespace.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/interface/component/mesh.hpp"

NAMESPACE_AXGL_INTERFACE

class Renderer
{
public:
  virtual ~Renderer() {}
  virtual void render(const Component& component) const = 0;
};

class RendererService : public Service
{
public:
  virtual std::shared_ptr<Renderer> create_renderer() = 0;
};

NAMESPACE_AXGL_INTERFACE_END
