#pragma once

#include <memory>

#include <axgl/common.hpp>
#include <axgl/interface/model.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/resource.hpp>

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "axgl/impl/assimp/model_loader.hpp"

NAMESPACE_AXGL_IMPL

class AssimpModelService : public interface::ModelService
{
private:
  std::shared_ptr<interface::RealmService> realm_service_;
  std::shared_ptr<interface::RendererService> renderer_service_;
  std::shared_ptr<interface::ResourceService> resource_service_;

public:
  void initialize() override
  {
    auto context = get_context();
    realm_service_ = context->axgl->realm_service();
    renderer_service_ = context->axgl->renderer_service();
    resource_service_ = context->axgl->resource_service();
  }

  ModelResources load_model(std::shared_ptr<interface::Component> root, const std::string& resource_key) override
  {
    ModelLoader loader(
      realm_service_,
      renderer_service_,
      resource_service_,
      root,
      resource_key,
      "default"
    );
    return loader.resources_;
  }
};

NAMESPACE_AXGL_IMPL_END
