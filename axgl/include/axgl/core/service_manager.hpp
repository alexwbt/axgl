#pragma once

#include <string>
#include <format>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "axgl/include.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/util/string.hpp"

NAMESPACE_AXGL

class ServiceManager final
{
private:
  using ServicePtr = std::shared_ptr<interface::Service>;

private:
  std::unordered_map<std::string, ServicePtr> services_;

public:
  void register_service(const std::string& id, ServicePtr service)
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

  ServicePtr get_service(const std::string& id)
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

  void update()
  {
    for (const auto& entry : services_)
      if (entry.second->running())
        entry.second->update();
  }

  void render()
  {
    for (const auto& entry : services_)
      if (entry.second->running())
        entry.second->render();
  }

  bool running()
  {
    for (const auto& entry : services_)
      if (entry.second->running())
        return true;
    return false;
  }

  void exec(const std::string& command)
  {
    auto args = util::split(command, ' ');
    if (args.empty())
      return;

    if (auto cs = dynamic_cast<interface::CommandService*>(get_service(args[0]).get()))
      cs->exec(args);
  }
};

NAMESPACE_AXGL_END
