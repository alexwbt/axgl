#pragma once

#include <vector>

#include <axgl/common.hpp>
#include <axgl/interface/components/mesh.hpp>
#include <axgl/interface/entity.hpp>
#include <axgl/interface/material.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/texture.hpp>

namespace axgl
{

class ModelService : virtual public axgl::Service
{
public:
  static constexpr std::string_view kTypeId = "service::model";

  struct ModelResources
  {
    std::vector<axgl::ptr_t<axgl::Texture>> textures;
    std::vector<axgl::ptr_t<axgl::Material>> materials;
    std::vector<axgl::ptr_t<axgl::component::Mesh>> meshes;
  };

  virtual axgl::ModelService::ModelResources load_model(
    axgl::ptr_t<axgl::Entity> entity, const std::string& resource_key) = 0;
};

} // namespace axgl

