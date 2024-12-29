#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include <axgl/common.hpp>
#include <axgl/interface/resource.hpp>

#include <bundlefile/unbundle.hpp>

NAMESPACE_AXGL_IMPL

class BundlefileResourceService : public interface::ResourceService
{
private:
  std::unordered_map<std::string, std::span<const uint8_t>> resources_;
  std::unordered_map<std::string, const std::vector<uint8_t>> resource_data_;
  std::unordered_map<std::string, std::unique_ptr<bundlefile::Bundle>> bundles_;

public:
  void load_resource(const std::string& key, std::span<const uint8_t> data) override
  {
    resources_[key] = data;
  }

  void load_resources(std::unordered_map<std::string, std::span<const uint8_t>> data) override
  {
    resources_.insert(data.begin(), data.end());
  }

  void unload_resource(const std::string& key) override
  {
    resources_.erase(key);
    resource_data_.erase(key);
  }

  void load_archive(const std::string& path) override
  {
    bundles_[path] = std::make_unique<bundlefile::Bundle>(path);

    auto bundle = bundles_[path]->get_bundle();
    for (const auto& file : *bundle->files())
      load_resource(file->key()->str(), flatbuffers::make_span(file->data()));
  }

  void unload_archive(const std::string& path) override
  {
    auto bundle = bundles_[path]->get_bundle();
    for (const auto& file : *bundle->files())
      unload_resource(file->key()->str());

    bundles_.erase(path);
  }

  bool has_resource(const std::string& key) const override
  {
    return resources_.contains(key);
  }

  const std::span<const uint8_t>& get_resource(const std::string& key) override
  {
#ifdef AXGL_DEBUG
    if (!has_resource(key))
      throw std::runtime_error("Resource service does not contain resource: " + key);
#endif
    return resources_.at(key);
  }
};

NAMESPACE_AXGL_IMPL_END
