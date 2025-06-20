#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/resource.hpp>

#include <bundlefile/unbundle.hpp>

NAMESPACE_AXGL_IMPL

class BundlefileService : public interface::Service
{
  std::shared_ptr<interface::ResourceService> resource_service_;
  std::unordered_map<std::string, std::unique_ptr<bundlefile::Bundle>> bundles_;

public:
  void initialize() override
  {
    const auto context = get_context();
    resource_service_ = context->axgl->resource_service();
  }

  void load_bundlefile(const std::string& path)
  {
    bundles_[path] = std::make_unique<bundlefile::Bundle>(path);

    const auto bundle = bundles_[path]->get_bundle();
    for (const auto& file : *bundle->files())
      resource_service_->load_resource(file->key()->str(), flatbuffers::make_span(file->data()));
  }

  void unload_bundlefile(const std::string& path)
  {
    const auto bundle = bundles_[path]->get_bundle();
    for (const auto& file : *bundle->files())
      resource_service_->unload_resource(file->key()->str());

    bundles_.erase(path);
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
inline std::shared_ptr<impl::BundlefileService> Axgl::use_service()
{
  auto bundlefile_service = std::make_shared<impl::BundlefileService>();
  register_service("bundlefile", bundlefile_service);

  return bundlefile_service;
}

NAMESPACE_AXGL_END
