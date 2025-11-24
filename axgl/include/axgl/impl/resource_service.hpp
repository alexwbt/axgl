#pragma once

#include <span>
#include <string>
#include <cstdint>
#include <unordered_map>

#include <axgl/axgl.hpp>
#include <axgl/common.hpp>
#include <axgl/interface/resource.hpp>

#include <axgl/impl/service.hpp>

namespace axgl
{
namespace impl
{

class ResourceService : public interface::ResourceService, public ServiceBase
{
  std::unordered_map<std::string, std::span<const uint8_t>> resources_;

public:
  void load_resource(const std::string& key, std::span<const uint8_t> data) override { resources_[key] = data; }

  void load_resources(std::unordered_map<std::string, std::span<const uint8_t>> data) override
  {
    resources_.insert(data.begin(), data.end());
  }

  void unload_resource(const std::string& key) override { resources_.erase(key); }

  [[nodiscard]] bool has_resource(const std::string& key) const override { return resources_.contains(key); }

  const std::span<const uint8_t>& get_resource(const std::string& key) override
  {
#ifdef AXGL_DEBUG
    if (!has_resource(key))
      throw std::runtime_error("Resource service does not contain resource: " + key);
#endif
    return resources_.at(key);
  }
};

} // namespace impl
} // namespace axgl

namespace axgl
{

template <> inline std::shared_ptr<impl::ResourceService> Axgl::use_service()
{
  auto resource_service = std::make_shared<impl::ResourceService>();
  register_service("resource", std::static_pointer_cast<interface::ResourceService>(resource_service));

  return resource_service;
}

} // namespace axgl
