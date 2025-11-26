#pragma once

#ifdef AXGL_DEBUG
#include <stdexcept>
#endif
#include <algorithm>
#include <format>
#include <memory>
#include <unordered_map>
#include <vector>

#include <axgl/interface/service.hpp>

#include <axgl/util/string.hpp>

namespace axgl
{

class ServiceContainer
{
  std::vector<std::shared_ptr<Service>> services_;
  std::unordered_map<std::string, std::shared_ptr<Service>> service_map_;

protected:
  Service::Context context_;

public:
  virtual ~ServiceContainer() = default;

  bool has_service(const std::string& id) const { return service_map_.contains(id); }

  void register_service(const std::string& id, std::shared_ptr<Service> service)
  {
#ifdef AXGL_DEBUG
    if (has_service(id))
      throw std::runtime_error(std::format("Trying to register service but service with id '{}' already exists.", id));
#endif
    service->set_context(&context_);
    service_map_.insert({id, service});
    services_.push_back(std::move(service));
  }

  void remove_service(const std::string& id)
  {
#ifdef AXGL_DEBUG
    if (!has_service(id))
      throw std::runtime_error(std::format("Trying to remove service but service with id '{}' does not exist.", id));
#endif
    std::erase_if(services_,
      [&](const auto& ptr)
      {
        return ptr == service_map_[id];
      });
    service_map_.erase(id);
  }

  template <typename ServiceType>
  bool has_service_type(const std::string& id) const
  {
    if (!has_service(id))
      return false;

    const auto& service = std::dynamic_pointer_cast<ServiceType>(service_map_.at(id));
    return service != nullptr;
  }

  template <typename ServiceType>
  std::shared_ptr<ServiceType> get_service(const std::string& id) const
  {
#ifdef AXGL_DEBUG
    if (!has_service(id))
      throw std::runtime_error(std::format("Service with id '{}' is required, but does not exist.", id));
#endif

    auto service = std::dynamic_pointer_cast<ServiceType>(service_map_.at(id));
#ifdef AXGL_DEBUG
    if (!service)
      throw std::runtime_error(
        std::format("Service type '{}' is required, but is not supported.", typeid(ServiceType).name()));
#endif

    return service;
  }

  void initialize() const
  {
    for (const auto& service : services_)
      service->initialize();
  }

  void terminate() const
  {
    for (const auto& service : services_)
      service->terminate();
  }

  void on_start() const
  {
    for (const auto& service : services_)
      service->on_start();
  }

  void on_end() const
  {
    for (const auto& service : services_)
      service->on_end();
  }

  void tick() const
  {
    for (const auto& service : services_)
      if (service->running())
        service->tick();
  }

  void update() const
  {
    for (const auto& service : services_)
      if (service->running())
        service->update();
  }

  void render() const
  {
    for (const auto& service : services_)
      if (service->running())
        service->render();
  }

  bool running() const
  {
    return std::ranges::any_of(services_,
      [&](const auto& service)
      {
        return service->keep_alive();
      });
  }

  void exec(const std::string& command) const
  {
    const auto args = util::split(command, ' ');
    if (args.empty())
      return;

    if (const auto service = get_service<Service>(args[0]))
      service->exec(args);
  }
};

} // namespace axgl
