#pragma once

#include <string>
#include <vector>
#include <memory>
#include <format>
#include <unordered_map>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/util/string.hpp>

NAMESPACE_AXGL_IMPL

class ServiceBase : virtual public interface::Service
{
  const interface::ServiceContext* context_ = nullptr;

public:
  void set_context(const interface::ServiceContext* context) override
  {
#ifdef AXGL_DEBUG
    if (context_)
      throw std::runtime_error("Service context already set.");
#endif
    context_ = context;
  }

protected:
  [[nodiscard]] const interface::ServiceContext* get_context() const override
  {
#ifdef AXGL_DEBUG
    if (!context_)
      throw std::runtime_error("ServiceContext is not provided here.");
#endif
    return context_;
  }
};

class ServiceManager
{
  std::vector<std::shared_ptr<interface::Service>> services_;
  std::unordered_map<std::string, std::shared_ptr<interface::Service>> service_map_;

protected:
  interface::ServiceContext context_;

public:
  virtual ~ServiceManager() { }

  bool has_service(const std::string& id) const { return service_map_.contains(id); }

  void register_service(const std::string& id, std::shared_ptr<interface::Service> service)
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
    std::erase_if(services_, [&](const auto& ptr) { return ptr == service_map_[id]; });
    service_map_.erase(id);
  }

  template <typename ServiceType> [[nodiscard]] bool has_service_type(const std::string& id) const
  {
    if (!has_service(id))
      return false;

    const auto& service = std::dynamic_pointer_cast<ServiceType>(service_map_.at(id));
    return service != nullptr;
  }

  template <typename ServiceType> [[nodiscard]] std::shared_ptr<ServiceType> get_service(const std::string& id) const
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
    return std::ranges::any_of(services_, [&](const auto& service) { return service->keep_alive(); });
  }

  void exec(const std::string& command) const
  {
    const auto args = util::split(command, ' ');
    if (args.empty())
      return;

    if (const auto service = get_service<interface::Service>(args[0]))
      service->exec(args);
  }
};

NAMESPACE_AXGL_IMPL_END
