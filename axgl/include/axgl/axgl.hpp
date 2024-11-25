#pragma once

#include <chrono>
#include <string>
#include <format>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "axgl/namespace.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/interface/window.hpp"
#include "axgl/interface/renderer.hpp"
#include "axgl/interface/realm.hpp"
#include "axgl/util/string.hpp"

NAMESPACE_AXGL

class Axgl final
{
private:
  std::unordered_map<std::string, std::shared_ptr<interface::Service>> services_;

private:
  void register_service(const std::string& id, std::shared_ptr<interface::Service> service)
  {
    if (services_.contains(id))
      throw std::runtime_error(std::format("Trying to register service but service with id '{}' already exists.", id));

    services_.insert({ id, std::move(service) });
  }

  void remove_service(const std::string& id)
  {
    if (!services_.contains(id))
      throw std::runtime_error(std::format("Trying to remove service but service with id '{}' does not exist.", id));

    services_.erase(id);
  }

  bool has_service(const std::string& id) const
  {
    return services_.contains(id);
  }

  template<typename ServiceType>
  bool has_service_type(const std::string& id) const
  {
    if (!has_service(id))
      return false;

    const auto& service = std::dynamic_pointer_cast<ServiceType>(services_.at(id));
    return service != nullptr;
  }

  template<typename ServiceType>
  std::shared_ptr<ServiceType> get_service(const std::string& id) const
  {
    if (!has_service(id))
      throw std::runtime_error(std::format("Service with id '{}' is required, but does not exist.", id));

    auto service = std::dynamic_pointer_cast<ServiceType>(services_.at(id));
    if (!service)
      throw std::runtime_error(std::format("Service type '{}' is required, but is not supported.", typeid(ServiceType).name()));

    return service;
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
        entry.second->update({});
  }

  void render()
  {
    for (const auto& entry : services_)
      if (entry.second->running())
        entry.second->render({});
  }

  bool running()
  {
    for (const auto& entry : services_)
      if (entry.second->keep_alive())
        return true;
    return false;
  }

  void exec(const std::string& command)
  {
    auto args = util::split(command, ' ');
    if (args.empty())
      return;

    if (auto service = get_service<interface::Service>(args[0]))
      service->exec(args);
  }

public:
  void run()
  {
    initialize();

    constexpr int64_t kOneSecond = 1000000000;
    constexpr double kTimeStep = kOneSecond / 60.0;

    auto start_time = std::chrono::high_resolution_clock::now();
    double delta_time = 0.0;

    while (running())
    {
      auto now = std::chrono::high_resolution_clock::now();
      delta_time += (double)std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_time).count() / kTimeStep;
      start_time = now;

      auto should_update = delta_time >= 1;

      while (delta_time >= 1)
      {
        update();
        delta_time--;
      }

      if (should_update)
        render();
    }

    terminate();
  }

  template<typename ServiceType>
  std::shared_ptr<ServiceType> use_service()
  {
    throw std::runtime_error(
      std::format("Service type '{}' is not supported.",
        typeid(ServiceType).name()));
  }

  template<typename ComponentType>
  std::shared_ptr<ComponentType> create_component()
  {
    throw std::runtime_error(
      std::format("Component type '{}' is not supported.",
        typeid(ComponentType).name()));
  }

  std::shared_ptr<interface::WindowService> window_service() const
  {
    return get_service<interface::WindowService>("window");
  }

  std::shared_ptr<interface::RendererService> renderer_service() const
  {
    return get_service<interface::RendererService>("renderer");
  }

  std::shared_ptr<interface::RealmService> realm_service() const
  {
    return get_service<interface::RealmService>("realm");
  }
};

NAMESPACE_AXGL_END
