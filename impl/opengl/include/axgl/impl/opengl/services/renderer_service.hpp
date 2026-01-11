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
#include <axgl/impl/opengl/materials/material_2d.hpp>
#include <axgl/impl/opengl/materials/phong_material.hpp>
#include <axgl/impl/opengl/renderer.hpp>
#include <axgl/impl/opengl/texture.hpp>

namespace axgl::impl::opengl
{

class RendererService : virtual public axgl::RendererService
{
  axgl::ptr_t<axgl::Renderer> renderer_;

public:
  void initialize(const Service::Context& context) override
  {
    // set glfw context
    axgl::impl::glfw::WindowService::set_window_hint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    axgl::impl::glfw::WindowService::set_window_hint(GLFW_CONTEXT_VERSION_MINOR, 1);
    axgl::impl::glfw::WindowService::set_window_hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  }

  void render(const Context& context) override
  {
    if (!renderer_)
    {
#ifdef AXGL_DEBUG
      AXGL_LOG_WARN("Active renderer not set.");
#endif
      return;
    }
    renderer_->render(context);
  }

  axgl::ptr_t<axgl::Renderer> create_renderer() override { return axgl::create_ptr<axgl::impl::opengl::Renderer>(); }

  axgl::ptr_t<axgl::Texture> create_texture() override { return axgl::create_ptr<axgl::impl::opengl::Texture>(); }

  axgl::ptr_t<axgl::Material> create_material(const std::string& type) override
  {
    if (type == "phong") return axgl::create_ptr<axgl::impl::opengl::PhongMaterial>();
    if (type == "2d") return axgl::create_ptr<axgl::impl::opengl::Material2d>();
#ifdef AXGL_DEBUG
    throw std::runtime_error("Unsupported material type: " + type);
#else
    return nullptr;
#endif
  }

  [[nodiscard]] axgl::ptr_t<axgl::Renderer> get_active_renderer() const override { return renderer_; }
  void set_active_renderer(axgl::ptr_t<axgl::Renderer> renderer) override { renderer_ = std::move(renderer); }
};

} // namespace axgl::impl::opengl
