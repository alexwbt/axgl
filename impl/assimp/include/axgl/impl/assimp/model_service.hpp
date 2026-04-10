#pragma once

#include <axgl/interface/services/model_service.hpp>

#include <axgl/impl/service_base.hpp>

#include "model_loader.hpp"

namespace axgl::impl::assimp
{

class ModelService : virtual public axgl::ModelService, public axgl::impl::ServiceBase
{
public:
  ModelResources load_model(const std::string& resource_key) override
  {
    const auto& entity_service = axgl_->entity_service();
    const auto& renderer_service = axgl_->renderer_service();
    const auto& resource_service = axgl_->resource_service();

    ModelLoader loader(entity_service, renderer_service, resource_service, resource_key, "3d");
    return loader.resources_;
  }
};

} // namespace axgl::impl::assimp
