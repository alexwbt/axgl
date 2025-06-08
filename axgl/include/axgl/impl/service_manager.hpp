#pragma once

#include <string>
#include <vector>
#include <memory>
#include <format>
#include <unordered_map>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/util/string.hpp>

NAMESPACE_AXGL

class ServiceManager : public interface::ServiceContextProvider
{
private:
  std::vector<std::shared_ptr<interface::Service>> services_;
  std::unordered_map<std::string, std::shared_ptr<interface::Service>> service_map_;

private:
  util::Iterable<std::shared_ptr<interface::Service>> services() const override
  {
    return util::to_iterable(services_);
  }

public:
  virtual ~ServiceManager() {}

  bool has_service(const std::string& id) const
  {
    return service_map_.contains(id);
  }

  void register_service(const std::string& id, std::shared_ptr<interface::Service> service)
  {
#ifdef AXGL_DEBUG
    if (has_service(id))
      throw std::runtime_error(std::format("Trying to register service but service with id '{}' already exists.", id));
#endif
    service_map_.insert({ id, service });
    services_.push_back(std::move(service));
  }

  void remove_service(const std::string& id)
  {
#ifdef AXGL_DEBUG
    if (!has_service(id))
      throw std::runtime_error(std::format("Trying to remove service but service with id '{}' does not exist.", id));
#endif
    services_.erase(
      std::remove_if(
        services_.begin(), services_.end(),
        [&](const auto& ptr) { return ptr == service_map_[id]; }),
      services_.end()
    );
    service_map_.erase(id);
  }

  template<typename ServiceType>
  bool has_service_type(const std::string& id) const
  {
    if (!has_service(id))
      return false;

    const auto& service = std::dynamic_pointer_cast<ServiceType>(service_map_.at(id));
    return service != nullptr;
  }

  template<typename ServiceType>
  std::shared_ptr<ServiceType> get_service(const std::string& id) const
  {
#ifdef AXGL_DEBUG
    if (!has_service(id))
      throw std::runtime_error(std::format("Service with id '{}' is required, but does not exist.", id));
#endif

    auto service = std::dynamic_pointer_cast<ServiceType>(service_map_.at(id));
#ifdef AXGL_DEBUG
    if (!service)
      throw std::runtime_error(std::format("Service type '{}' is required, but is not supported.", typeid(ServiceType).name()));
#endif

    return service;
  }

  void initialize(Axgl* axgl)
  {
    interface::ServiceContext context(this, axgl);
    for (const auto& service : services_)
      service->initialize();
  }

  void terminate(Axgl* axgl)
  {
    interface::ServiceContext context(this, axgl);
    for (const auto& service : services_)
      service->terminate();
  }

  void tick(Axgl* axgl)
  {
    interface::ServiceContext context(this, axgl);
    for (const auto& service : services_)
      if (service->running())
        service->tick();
  }

  void update(Axgl* axgl)
  {
    interface::ServiceContext context(this, axgl);
    for (const auto& service : services_)
      if (service->running())
        service->update();
  }

  void render(Axgl* axgl)
  {
    interface::ServiceContext context(this, axgl);
    for (const auto& service : services_)
      if (service->running())
        service->render();
  }

  bool running(Axgl* axgl)
  {
    interface::ServiceContext context(this, axgl);
    for (const auto& service : services_)
      if (service->keep_alive())
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
