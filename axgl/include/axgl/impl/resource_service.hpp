#pragma once

#include "axgl/except.hpp"
#include "axgl/namespace.hpp"
#include "axgl/interface/resource.hpp"

#include "axgl/util/file.hpp"

NAMESPACE_AXGL_IMPL

class ResourceService : public interface::ResourceService
{
private:
  std::unordered_map<std::string, std::span<const uint8_t>> resources_;
  std::unordered_map<std::string, const std::vector<uint8_t>> resource_data_;

public:
  void load_resources(const std::string& path)
  {
    resource_data_.insert({ path, util::read_file(path) });
    resources_[path] = resource_data_[path];
  }

  void unload_resource(const std::string& key)
  {
    resources_.erase(key);
    resource_data_.erase(key);
  }

  const std::span<const uint8_t>& get_resource(const std::string& key) override
  {
#ifdef AXGL_DEBUG
    if (!resources_.contains(key))
      throw std::runtime_error("Resource service does not contain resource: " + key);
#endif
    return resources_.at(key);
  }

  void load_resource_from_mem(const std::string& key, std::span<const uint8_t> data) override
  {
    resources_[key] = data;
  }

  void load_resources_from_mem(
    const std::string& key_prefix,
    const std::unordered_map<std::string, std::span<const uint8_t>>& resources
  ) override
  {
    for (const auto& [key, value] : resources)
      resources_[key_prefix + key] = value;
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::ResourceService> Axgl::use_service()
{
  auto resource_service = std::make_shared<impl::ResourceService>();
  register_service("resource", resource_service);

  return resource_service;
}

NAMESPACE_AXGL_END
