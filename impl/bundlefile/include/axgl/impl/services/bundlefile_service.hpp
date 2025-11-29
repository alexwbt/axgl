#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <axgl/interface/service.hpp>
#include <axgl/interface/services/resource_service.hpp>

#include <axgl/axgl.hpp>

#include <bundlefile/unbundle.hpp>

namespace axgl::impl::bundlefile
{

class BundlefileService : public axgl::Service
{
  std::shared_ptr<axgl::ResourceService> resource_service_;
  std::unordered_map<std::string, std::unique_ptr<::bundlefile::Bundle>> bundles_;

public:
  void initialize(const Service::Context& context) override { resource_service_ = context.axgl.resource_service(); }

  void load_bundlefile(const std::string& path)
  {
    bundles_[path] = std::make_unique<::bundlefile::Bundle>(path);

    for (const auto bundle = bundles_[path]->get_bundle(); const auto& file : *bundle->files())
      resource_service_->load_resource(file->key()->str(), flatbuffers::make_span(file->data()));
  }

  void unload_bundlefile(const std::string& path)
  {
    for (const auto bundle = bundles_[path]->get_bundle(); const auto& file : *bundle->files())
      resource_service_->unload_resource(file->key()->str());

    bundles_.erase(path);
  }
};

} // namespace axgl::impl::bundlefile
