#pragma once

#include <memory>
#include <vector>

#include <axgl/common.hpp>
#include <axgl/interface/component/mesh.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/renderer.hpp>

NAMESPACE_AXGL_INTERFACE

class ModelService : virtual public Service
{
public:
  struct ModelResources
  {
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<Material>> materials;
    std::vector<std::shared_ptr<component::Mesh>> meshes;
  };

  virtual ModelResources load_model(std::shared_ptr<Entity> entity, const std::string& resource_key) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
