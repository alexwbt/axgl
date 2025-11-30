#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include <axgl/interface/components/mesh.hpp>
#include <axgl/interface/entity.hpp>
#include <axgl/interface/material.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/texture.hpp>

namespace axgl
{

class ModelService : virtual public Service
{
public:
  static constexpr std::string_view kTypeId = "service::model";

  struct ModelResources
  {
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<Material>> materials;
    std::vector<std::shared_ptr<component::Mesh>> meshes;
  };

  virtual ModelResources load_model(std::shared_ptr<Entity> entity, const std::string& resource_key) = 0;
};

} // namespace axgl

