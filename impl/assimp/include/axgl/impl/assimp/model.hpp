#pragma once

#include <memory>

#include <axgl/interface/model.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/resource.hpp>
#include <axgl/impl/service.hpp>

#ifndef AXGL_DEFINED_CREATE_COMPONENT_MESH
#error Implementation of interface::Mesh must be defined before using __FILE__
#endif

#include "./model_loader.hpp"

NAMESPACE_AXGL_IMPL

class AssimpModelService : virtual public interface::ModelService, public ServiceBase
{
  std::shared_ptr<interface::RealmService> realm_service_;
  std::shared_ptr<interface::RendererService> renderer_service_;
  std::shared_ptr<interface::ResourceService> resource_service_;

public:
  void initialize() override
  {
    const auto context = get_context();
    realm_service_ = context->axgl->realm_service();
    renderer_service_ = context->axgl->renderer_service();
    resource_service_ = context->axgl->resource_service();
  }

  ModelResources load_model(std::shared_ptr<interface::Entity> entity, const std::string& resource_key) override
  {
    ModelLoader loader(realm_service_, renderer_service_, resource_service_, entity, resource_key, "default");
    return loader.resources_;
  }
};

NAMESPACE_AXGL_IMPL_END
