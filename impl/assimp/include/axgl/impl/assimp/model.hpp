#pragma once

#include <memory>

#include <axgl/impl/service.hpp>
#include <axgl/interface/services/model_service.hpp>
#include <axgl/interface/services/realm_service.hpp>
#include <axgl/interface/services/renderer_service.hpp>
#include <axgl/interface/services/resource_service.hpp>

#ifndef AXGL_DEFINED_CREATE_COMPONENT_MESH
#error Implementation of interface::Mesh must be defined before using __FILE__
#endif

#include "./model_loader.hpp"

namespace axgl::impl
{

class AssimpModelService : virtual public axgl::ModelService, public ServiceBase
{
  std::shared_ptr<axgl::RealmService> realm_service_;
  std::shared_ptr<axgl::RendererService> renderer_service_;
  std::shared_ptr<axgl::ResourceService> resource_service_;

public:
  void initialize() override
  {
    const auto context = get_context();
    realm_service_ = context->axgl->realm_service();
    renderer_service_ = context->axgl->renderer_service();
    resource_service_ = context->axgl->resource_service();
  }

  ModelResources load_model(std::shared_ptr<axgl::Entity> entity, const std::string& resource_key) override
  {
    ModelLoader loader(realm_service_, renderer_service_, resource_service_, entity, resource_key, "default");
    return loader.resources_;
  }
};

} // namespace axgl::impl

