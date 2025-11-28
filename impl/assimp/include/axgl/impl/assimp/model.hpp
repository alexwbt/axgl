#pragma once

#include <memory>

#include <axgl/impl/service_base.hpp>
#include <axgl/interface/services/entity_service.hpp>
#include <axgl/interface/services/model_service.hpp>
#include <axgl/interface/services/renderer_service.hpp>
#include <axgl/interface/services/resource_service.hpp>

#include "./model_loader.hpp"

namespace axgl::impl::assimp
{

class ModelService : virtual public axgl::ModelService, public ServiceBase
{
  std::shared_ptr<axgl::EntityService> entity_service_;
  std::shared_ptr<axgl::RendererService> renderer_service_;
  std::shared_ptr<axgl::ResourceService> resource_service_;

public:
  void initialize() override
  {
    const auto context = get_context();
    entity_service_ = context->axgl->entity_service();
    renderer_service_ = context->axgl->renderer_service();
    resource_service_ = context->axgl->resource_service();
  }

  ModelResources load_model(const std::shared_ptr<axgl::Entity> entity, const std::string& resource_key) override
  {
    ModelLoader loader(entity_service_, renderer_service_, resource_service_, entity, resource_key, "default");
    return loader.resources_;
  }
};

} // namespace axgl::impl::assimp
