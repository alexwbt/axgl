#pragma once

#include <span>
#include <vector>
#include <memory>
#include <stdint.h>

#include "axgl/common.hpp"
#include "axgl/interface/realm.hpp"
#include "axgl/interface/renderer.hpp"
#include "axgl/interface/component/mesh.hpp"

NAMESPACE_AXGL_INTERFACE

class ModelService : public Service
{
public:
  struct ModelResources
  {
    std::vector<std::shared_ptr<interface::Mesh>> meshes;
    std::vector<std::shared_ptr<interface::Texture>> textures;
    std::vector<std::shared_ptr<interface::Material>> materials;
  };

  virtual ModelResources load_model(std::shared_ptr<interface::Component> root, const std::string& resource_key) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
