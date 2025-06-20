#pragma once

#include <span>
#include <string>
#include <cstdint>
#include <unordered_map>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>

NAMESPACE_AXGL_INTERFACE

class ResourceService : public Service
{
public:
  virtual void load_resource(const std::string& key, std::span<const uint8_t> data) = 0;
  virtual void load_resources(std::unordered_map<std::string, std::span<const uint8_t>> data) = 0;
  virtual void unload_resource(const std::string& key) = 0;

  [[nodiscard]] virtual bool has_resource(const std::string& key) const = 0;
  virtual const std::span<const uint8_t>& get_resource(const std::string& key) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
