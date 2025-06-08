#pragma once

#include <span>
#include <vector>
#include <memory>
#include <stdint.h>

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/component/mesh.hpp>

NAMESPACE_AXGL_INTERFACE

class ModelService : public Service
{
public:
  struct ModelResources
  {
    std::vector<std::shared_ptr<interface::Texture>> textures;
    std::vector<std::shared_ptr<interface::Material>> materials;
    std::vector<std::shared_ptr<interface::component::Mesh>> meshes;
  };

  virtual ModelResources load_model(std::shared_ptr<interface::Entity> entity, const std::string& resource_key) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
