#pragma once

#include <string_view>

#include <axgl/common.hpp>
#include <axgl/interface/material.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/texture.hpp>

namespace axgl
{

class RendererService : virtual public axgl::Service
{
public:
  static constexpr std::string_view kTypeId = "service::renderer";

  virtual axgl::ptr_t<axgl::Renderer> create_renderer() = 0;
  virtual axgl::ptr_t<axgl::Texture> create_texture() = 0;
  virtual axgl::ptr_t<axgl::Material> create_material(const std::string& type) = 0;
};

} // namespace axgl
