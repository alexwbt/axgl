#pragma once

#include <unordered_map>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class ResourceService : virtual public axgl::Service
{
public:
  static constexpr std::string_view kTypeId = "service::resource";

  virtual void load_resource(const std::string& key, std::span<const std::uint8_t> data) = 0;
  virtual void load_resources(std::unordered_map<std::string, std::span<const std::uint8_t>> data) = 0;
  virtual void unload_resource(const std::string& key) = 0;

  [[nodiscard]] virtual bool has_resource(const std::string& key) const = 0;
  virtual const std::span<const std::uint8_t>& get_resource(const std::string& key) = 0;
};

} // namespace axgl

