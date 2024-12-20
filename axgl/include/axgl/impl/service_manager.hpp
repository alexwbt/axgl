#pragma once

#include <string>
#include <memory>
#include <ranges>
#include <format>
#include <unordered_map>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/util/string.hpp>

NAMESPACE_AXGL

class ServiceManager : public interface::ServiceContextProvider
{
private:
  std::unordered_map<std::string, std::shared_ptr<interface::Service>> services_;

private:
  util::Iterable<std::shared_ptr<interface::Service>> services() const override
  {
    return util::to_iterable(std::views::values(services_));
  }

public:
  virtual ~ServiceManager() {}

  void register_service(const std::string& id, std::shared_ptr<interface::Service> service)
  {
#ifdef AXGL_DEBUG
    if (services_.contains(id))
      throw std::runtime_error(std::format("Trying to register service but service with id '{}' already exists.", id));
#endif
    services_.insert({ id, std::move(service) });
  }

  void remove_service(const std::string& id)
  {
#ifdef AXGL_DEBUG
    if (!services_.contains(id))
      throw std::runtime_error(std::format("Trying to remove service but service with id '{}' does not exist.", id));
#endif
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
#ifdef AXGL_DEBUG
    if (!has_service(id))
      throw std::runtime_error(std::format("Service with id '{}' is required, but does not exist.", id));
#endif

    auto service = std::dynamic_pointer_cast<ServiceType>(services_.at(id));
#ifdef AXGL_DEBUG
    if (!service)
      throw std::runtime_error(std::format("Service type '{}' is required, but is not supported.", typeid(ServiceType).name()));
#endif

    return service;
  }

  void initialize(Axgl* axgl)
  {
    interface::ServiceContext context(this, axgl);

    for (const auto& entry : services_)
      entry.second->initialize();
  }

  void terminate(Axgl* axgl)
  {
    interface::ServiceContext context(this, axgl);

    for (const auto& entry : services_)
      entry.second->terminate();
  }

  void update(Axgl* axgl)
  {
    interface::ServiceContext context(this, axgl);

    for (const auto& entry : services_)
      if (entry.second->running())
        entry.second->update();
  }

  void render(Axgl* axgl)
  {
    interface::ServiceContext context(this, axgl);

    for (const auto& entry : services_)
      if (entry.second->running())
        entry.second->render();
  }

  bool running(Axgl* axgl)
  {
    interface::ServiceContext context(this, axgl);

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
};

NAMESPACE_AXGL_END
