#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/services/entity_service.hpp>
#include <axgl/interface/services/model_service.hpp>
#include <axgl/interface/services/renderer_service.hpp>
#include <axgl/interface/services/resource_service.hpp>

#include "model_loader.hpp"

namespace axgl::impl::assimp
{

class ModelService : virtual public axgl::ModelService
{
  axgl::ptr_t<axgl::EntityService> entity_service_;
  axgl::ptr_t<axgl::RendererService> renderer_service_;
  axgl::ptr_t<axgl::ResourceService> resource_service_;

public:
  void initialize(const Service::Context& context) override
  {
    entity_service_ = context.axgl.entity_service();
    renderer_service_ = context.axgl.renderer_service();
    resource_service_ = context.axgl.resource_service();
  }

  ModelResources load_model(const axgl::ptr_t<axgl::Entity> entity, const std::string& resource_key) override
  {
    ModelLoader loader(entity_service_, renderer_service_, resource_service_, entity, resource_key, "phong");
    return loader.resources_;
  }
};

} // namespace axgl::impl::assimp
