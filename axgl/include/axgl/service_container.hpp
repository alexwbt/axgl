#pragma once

#include <algorithm>
#include <format>
#include <memory>
#include <ranges>
#include <unordered_map>

#include <util/string.hpp>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class ServiceContainer
{
  std::unordered_map<std::string, axgl::ptr_t<Service>> service_map_;

public:
  virtual ~ServiceContainer() = default;

  [[nodiscard]] auto services() const { return service_map_ | std::views::values; }

  [[nodiscard]] virtual bool has_service(const std::string& type_id) const { return service_map_.contains(type_id); }

  virtual void register_service(const std::string& type_id, const axgl::ptr_t<Service>& service)
  {
#ifdef AXGL_DEBUG
    if (has_service(type_id))
      throw std::runtime_error(
        std::format("Trying to register service but service with id '{}' already exists.", type_id));
#endif
    service_map_.emplace(type_id, service);
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
  axgl::ptr_t<ServiceType> register_service_t()
  {
    auto service = axgl::create_ptr<ServiceType>();
    register_service(ServiceType::kTypeId.data(), service);
    return service;
  }

  template <typename ServiceType>
  [[nodiscard]] bool has_service_type(const std::string& type_id) const
  {
    if (!has_service(type_id)) return false;

    const auto& service = std::dynamic_pointer_cast<ServiceType>(service_map_.at(type_id));
    return service != nullptr;
  }

  template <typename ServiceType>
  [[nodiscard]] axgl::ptr_t<ServiceType> get_service(const std::string& type_id) const
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
  [[nodiscard]] axgl::ptr_t<ServiceType> get_service_t() const
  {
    return get_service<ServiceType>(ServiceType::kTypeId.data());
  }

  virtual void set_context(const axgl::Context* context) const
  {
    for (const auto& service : services())
      service->set_context(context);
  }

  virtual void initialize() const
  {
    for (const auto& service : services())
      service->initialize();
  }

  virtual void terminate() const
  {
    for (const auto& service : services())
      service->terminate();
  }

  virtual void on_start() const
  {
    for (const auto& service : services())
      service->on_start();
  }

  virtual void on_end() const
  {
    for (const auto& service : services())
      service->on_end();
  }

  virtual void tick() const
  {
    for (const auto& service : services())
      if (service->running()) service->tick();
  }

  virtual void update() const
  {
    for (const auto& service : services())
      if (service->running()) service->update();
  }

  virtual void render() const
  {
    for (const auto& service : services())
      if (service->running()) service->render();
  }

  [[nodiscard]] virtual bool running() const
  {
    return std::ranges::any_of(services(), [&](const auto& service) { return service->keep_alive(); });
  }

  virtual void exec(const std::string& command) const
  {
    const auto args = util::split_string(command, ' ');
    if (args.empty()) return;

    if (const auto service = get_service<Service>(args[0])) service->exec(args);
  }
};

} // namespace axgl
