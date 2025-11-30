#pragma once

#include <memory>
#include <string_view>

#include <axgl/interface/material.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/texture.hpp>

namespace axgl
{

class RendererService : virtual public Service
{
public:
  static constexpr std::string_view kTypeId = "service::renderer";

  virtual std::shared_ptr<Renderer> create_renderer() = 0;
  virtual std::shared_ptr<Texture> create_texture() = 0;
  virtual std::shared_ptr<Material> create_material(const std::string& type) = 0;
};

} // namespace axgl
