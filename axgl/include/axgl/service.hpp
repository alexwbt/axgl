#pragma once

#include <string>
#include <format>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "axgl/index.hpp"

NAMESPACE_AXGL

class Service
{
public:
  virtual ~Service() {}
  virtual void initialize() = 0;
  virtual void terminate() = 0;
};

class ServiceManager final
{
private:
  std::unordered_map<std::string, std::shared_ptr<Service>> services_;

public:
  void register_service(const std::string& id, std::shared_ptr<Service> service)
  {
    if (services_.contains(id))
      throw std::runtime_error(std::format("Service with id '{}' already exists.", id));

    services_.insert({ id, std::move(service) });
  }

  void remove_service(const std::string& id)
  {
    if (!services_.contains(id))
      throw std::runtime_error(std::format("Service with id '{}' does not exist.", id));

    services_.erase(id);
  }

  std::shared_ptr<Service> get_service(const std::string& id)
  {
    if (!services_.contains(id))
      throw std::runtime_error(std::format("Service with id '{}' is required, but does not exist.", id));

    return services_[id];
  }

  void initialize()
  {
    for (const auto& entry : services_)
      entry.second->initialize();
  }

  void terminate()
  {
    for (const auto& entry : services_)
      entry.second->terminate();
  }
};

NAMESPACE_AXGL_END
