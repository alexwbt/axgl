#pragma once

#ifdef AXGL_DEBUG
#include <stdexcept>
#endif
#include <memory>
#include <string>

#include <axgl/interface/material.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/services/renderer_service.hpp>
#include <axgl/interface/texture.hpp>

#include <axgl/impl/glfw.hpp>
#include <axgl/impl/opengl/component/mesh.hpp>
#include <axgl/impl/opengl/material.hpp>
#include <axgl/impl/opengl/renderer.hpp>
#include <axgl/impl/opengl/texture.hpp>
#include <axgl/impl/service_base.hpp>

namespace axgl::impl
{

class OpenglRendererService : virtual public axgl::RendererService, public ServiceBase
{
public:
  void initialize() override
  {
    const auto axgl = get_context()->axgl;

    // set glfw context
    const auto window_service = axgl->get_service<impl::GlfwWindowService>(DefaultServices::kWindow);
    window_service->set_window_hint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    window_service->set_window_hint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window_service->set_window_hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // register mesh component
    const auto entity_service = axgl->entity_service();
    entity_service->register_component_t<component::OpenglMesh>();
  }

  std::shared_ptr<axgl::Renderer> create_renderer() override { return std::make_shared<OpenglRenderer>(); }

  std::shared_ptr<axgl::Texture> create_texture() override { return std::make_shared<OpenglTexture>(); }

  std::shared_ptr<axgl::Material> create_material(const std::string& type) override
  {
    if (type == "2d")
      return std::make_shared<OpenglDefault2DMaterial>();

    if (type == "default")
      return std::make_shared<OpenglDefaultMaterial>();

#ifdef AXGL_DEBUG
    throw std::runtime_error("Unsupported material type: " + type);
#else
    return nullptr;
#endif
  }
};
} // namespace axgl::impl
