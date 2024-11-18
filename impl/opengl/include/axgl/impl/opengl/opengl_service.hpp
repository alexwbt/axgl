#pragma once

#include "axgl/axgl.hpp"
#include "axgl/namespace.hpp"
#include "axgl/interface/renderer.hpp"
#include "axgl/interface/component/component.hpp"

NAMESPACE_AXGL_IMPL

class OpenglRenderer : public interface::Renderer
{
public:
  void render(const interface::Component& component) const override
  {
    // glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    component.render();
  }
};

class OpenglRendererService : public interface::RendererService
{
public:
  void initialize() override {}
  void terminate() override {}
  void update() override {}
  void render() override {}
  bool running() override { return false; }

  std::shared_ptr<interface::Renderer> create_renderer() override
  {
    return std::make_shared<OpenglRenderer>();
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::OpenglRendererService> Axgl::use_service()
{
  auto glfw_service = std::make_shared<impl::OpenglRendererService>();
  register_service("renderer", glfw_service);

  return glfw_service;
}

NAMESPACE_AXGL_END

// component implementations
#include "axgl/impl/opengl/component/mesh.hpp"
