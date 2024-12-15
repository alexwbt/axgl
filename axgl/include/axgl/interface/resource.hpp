#pragma once

#include <string>
#include <stdint.h>
#include <unordered_map>

#include "axgl/namespace.hpp"
#include "axgl/interface/service.hpp"

NAMESPACE_AXGL_INTERFACE

class ResourceService : public Service
{
public:
  virtual void load_resource(const std::string& key, std::span<const uint8_t> data) = 0;
  virtual void load_resources(const std::string& key_prefix,
    const std::unordered_map<std::string, std::span<const uint8_t>>& resources) = 0;

  virtual const std::span<const uint8_t>& get_resource(const std::string& key) = 0;
  virtual void unload_resource(const std::string& key) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
