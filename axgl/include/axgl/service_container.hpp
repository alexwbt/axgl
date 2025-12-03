#pragma once

#include <algorithm>
#include <format>
#include <memory>
#include <ranges>
#include <unordered_map>
#include <vector>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>

#include <axgl/util/string.hpp>

namespace axgl
{

class ServiceContainer
{
  std::unordered_map<std::string, ptr_t<Service>> service_map_;

public:
  virtual ~ServiceContainer() = default;

  [[nodiscard]] auto services() const { return service_map_ | std::views::values; }

  [[nodiscard]] virtual bool has_service(const std::string& type_id) const { return service_map_.contains(type_id); }

  virtual void register_service(const std::string& type_id, ptr_t<Service> service)
  {
#ifdef AXGL_DEBUG
    if (has_service(type_id))
      throw std::runtime_error(
        std::format("Trying to register service but service with id '{}' already exists.", type_id));
#endif
    service_map_.insert({type_id, service});
  }

  virtual void remove_service(const std::string& type_id)
  {
#ifdef AXGL_DEBUG
    if (!has_service(type_id))
      throw std::runtime_error(
        std::format("Trying to remove service but service with id '{}' does not exist.", type_id));
#endif
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
  [[nodiscard]] ptr_t<ServiceType> get_service(const std::string& type_id) const
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
  [[nodiscard]] ptr_t<ServiceType> get_service_t() const
  {
    return get_service<ServiceType>(ServiceType::kTypeId.data());
  }

  virtual void initialize(const Service::Context& context) const
  {
    for (const auto& service : services())
      service->initialize(context);
  }

  virtual void terminate(const Service::Context& context) const
  {
    for (const auto& service : services())
      service->terminate(context);
  }

  virtual void on_start(const Service::Context& context) const
  {
    for (const auto& service : services())
      service->on_start(context);
  }

  virtual void on_end(const Service::Context& context) const
  {
    for (const auto& service : services())
      service->on_end(context);
  }

  virtual void tick(const Service::Context& context) const
  {
    for (const auto& service : services())
      if (service->running(context))
        service->tick(context);
  }

  virtual void update(const Service::Context& context) const
  {
    for (const auto& service : services())
      if (service->running(context))
        service->update(context);
  }

  virtual void render(const Service::Context& context) const
  {
    for (const auto& service : services())
      if (service->running(context))
        service->render(context);
  }

  [[nodiscard]] virtual bool running(const Service::Context& context) const
  {
    return std::ranges::any_of(services(), [&](const auto& service) { return service->keep_alive(context); });
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
