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
  void load_resource(const std::string& key, std::span<const uint8_t> data) override
  {
    resources_[key] = data;
  }

  void load_resources(
    const std::string& key_prefix,
    const std::unordered_map<std::string, std::span<const uint8_t>>& resources
  ) override
  {
    for (const auto& [key, value] : resources)
      resources_[key_prefix + key] = value;
  }

  const std::span<const uint8_t>& get_resource(const std::string& key) override
  {
#ifdef AXGL_DEBUG
    if (!resources_.contains(key))
      throw std::runtime_error("Resource service does not contain resource: " + key);
#endif
    return resources_.at(key);
  }

  void unload_resource(const std::string& key) override
  {
    resources_.erase(key);
    resource_data_.erase(key);
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
