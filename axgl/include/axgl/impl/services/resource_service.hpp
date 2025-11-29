#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <unordered_map>

#include <axgl/interface/services/resource_service.hpp>

namespace axgl::impl
{

class ResourceService : virtual public axgl::ResourceService
{
  std::unordered_map<std::string, std::span<const std::uint8_t>> resources_;

public:
  void load_resource(const std::string& key, const std::span<const std::uint8_t> data) override
  {
    resources_[key] = data;
  }

  void load_resources(std::unordered_map<std::string, std::span<const std::uint8_t>> data) override
  {
    resources_.insert(data.begin(), data.end());
  }

  void unload_resource(const std::string& key) override { resources_.erase(key); }

  [[nodiscard]] bool has_resource(const std::string& key) const override { return resources_.contains(key); }

  const std::span<const std::uint8_t>& get_resource(const std::string& key) override
  {
#ifdef AXGL_DEBUG
    if (!has_resource(key))
      throw std::runtime_error("Resource service does not contain resource: " + key);
#endif
    return resources_.at(key);
  }
};

} // namespace axgl::impl
