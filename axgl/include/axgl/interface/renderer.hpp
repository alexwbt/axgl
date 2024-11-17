#pragma once

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
  virtual std::shared_ptr<Renderer> get_renderer() = 0;
  virtual std::shared_ptr<Mesh2D> create_mesh2d() = 0;
  virtual std::shared_ptr<Mesh3D> create_mesh3d() = 0;
};

NAMESPACE_AXGL_INTERFACE_END
