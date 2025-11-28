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
#include <axgl/impl/opengl/components/mesh.hpp>
#include <axgl/impl/opengl/material.hpp>
#include <axgl/impl/opengl/materials/default_2d_material.hpp>
#include <axgl/impl/opengl/materials/default_material.hpp>
#include <axgl/impl/opengl/renderer.hpp>
#include <axgl/impl/opengl/texture.hpp>
#include <axgl/impl/service_base.hpp>

namespace axgl::impl::opengl
{

class RendererService : virtual public axgl::RendererService, public impl::ServiceBase
{
public:
  void initialize() override
  {
    const auto axgl = get_context()->axgl;

    // set glfw context
    const auto window_service = axgl->get_service<impl::glfw::WindowService>(DefaultServices::kWindow);
    window_service->set_window_hint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    window_service->set_window_hint(GLFW_CONTEXT_VERSION_MINOR, 3);
    window_service->set_window_hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // register mesh component
    const auto entity_service = axgl->entity_service();
    entity_service->register_component_t<impl::opengl::component::Mesh>();
  }

  std::shared_ptr<axgl::Renderer> create_renderer() override { return std::make_shared<Renderer>(); }

  std::shared_ptr<axgl::Texture> create_texture() override { return std::make_shared<Texture>(); }

  std::shared_ptr<axgl::Material> create_material(const std::string& type) override
  {
    if (type == "2d")
      return std::make_shared<impl::opengl::Default2DMaterial>();

    if (type == "default")
      return std::make_shared<impl::opengl::DefaultMaterial>();

#ifdef AXGL_DEBUG
    throw std::runtime_error("Unsupported material type: " + type);
#else
    return nullptr;
#endif
  }
};
} // namespace axgl::impl::opengl
