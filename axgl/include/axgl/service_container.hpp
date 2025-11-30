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

public:
  virtual ~ServiceContainer() = default;

  [[nodiscard]] virtual bool has_service(const std::string& type_id) const { return service_map_.contains(type_id); }

  virtual void register_service(const std::string& type_id, std::shared_ptr<Service> service)
  {
#ifdef AXGL_DEBUG
    if (has_service(type_id))
      throw std::runtime_error(
        std::format("Trying to register service but service with id '{}' already exists.", type_id));
#endif
    service_map_.insert({type_id, service});
    services_.push_back(std::move(service));
  }

  virtual void remove_service(const std::string& type_id)
  {
#ifdef AXGL_DEBUG
    if (!has_service(type_id))
      throw std::runtime_error(
        std::format("Trying to remove service but service with id '{}' does not exist.", type_id));
#endif
    std::erase_if(
      services_, [&](const auto& ptr)
    {
      return ptr == service_map_[type_id];
    });
    service_map_.erase(type_id);
  }

  template <typename ServiceType>
  [[nodiscard]] bool has_service_type(const std::string& type_id) const
  {
    if (!has_service(type_id))
      return false;

    const auto& service = std::dynamic_pointer_cast<ServiceType>(service_map_.at(type_id));
    return service != nullptr;
  }

  template <typename ServiceType>
  [[nodiscard]] std::shared_ptr<ServiceType> get_service(const std::string& type_id) const
  {
#ifdef AXGL_DEBUG
    if (!has_service(type_id))
      throw std::runtime_error(std::format("Service with id '{}' is required, but does not exist.", type_id));
#endif

    auto service = std::dynamic_pointer_cast<ServiceType>(service_map_.at(type_id));
#ifdef AXGL_DEBUG
    if (!service)
      throw std::runtime_error(
        std::format("Service type '{}' is required, but is not supported.", typeid(ServiceType).name()));
#endif

    return service;
  }

  template <typename ServiceType>
  [[nodiscard]] std::shared_ptr<ServiceType> get_service_t() const
  {
    return get_service<ServiceType>(ServiceType::kTypeId.data());
  }

  virtual void initialize(const Service::Context& context) const
  {
    for (const auto& service : services_)
      service->initialize(context);
  }

  virtual void terminate(const Service::Context& context) const
  {
    for (const auto& service : services_)
      service->terminate(context);
  }

  virtual void on_start(const Service::Context& context) const
  {
    for (const auto& service : services_)
      service->on_start(context);
  }

  virtual void on_end(const Service::Context& context) const
  {
    for (const auto& service : services_)
      service->on_end(context);
  }

  virtual void tick(const Service::Context& context) const
  {
    for (const auto& service : services_)
      if (service->running(context))
        service->tick(context);
  }

  virtual void update(const Service::Context& context) const
  {
    for (const auto& service : services_)
      if (service->running(context))
        service->update(context);
  }

  virtual void render(const Service::Context& context) const
  {
    for (const auto& service : services_)
      if (service->running(context))
        service->render(context);
  }

  [[nodiscard]] virtual bool running(const Service::Context& context) const
  {
    return std::ranges::any_of(
      services_, [&](const auto& service)
    {
      return service->keep_alive(context);
    });
  }

  virtual void exec(const Service::Context& context, const std::string& command) const
  {
    const auto args = util::split(command, ' ');
    if (args.empty())
      return;

    if (const auto service = get_service<Service>(args[0]))
      service->exec(context, args);
  }
};

} // namespace axgl
