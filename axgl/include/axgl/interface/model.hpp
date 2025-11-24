#pragma once

#include <vector>
#include <memory>

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/component/mesh.hpp>

namespace axgl
{
namespace interface
{

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

} // namespace interface
} // namespace axgl
