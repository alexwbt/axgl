#pragma once

#include "axgl/axgl.hpp"
#include "axgl/namespace.hpp"
#include "axgl/interface/renderer.hpp"

NAMESPACE_AXGL_IMPL

class OpenglRenderer : public interface::Renderer
{
public:
  void render(const interface::Renderable& renderable) override
  {

  }
};

class OpenglService : public interface::RenderService
{
public:

};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

NAMESPACE_AXGL_END
