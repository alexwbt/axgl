#pragma once

#include <memory>
#include <vector>

#include <axgl/common.hpp>
#include <axgl/util/iterable.hpp>

NAMESPACE_AXGL
class Axgl;
NAMESPACE_AXGL_END

NAMESPACE_AXGL_INTERFACE

class ServiceContext;
class ServiceContextProvider;
class Service
{
  const ServiceContext* context_ = nullptr;
  friend class ServiceContextProvider;

protected:
  [[nodiscard]] const ServiceContext* get_context() const
  {
#ifdef AXGL_DEBUG
    if (!context_)
      throw std::runtime_error("ServiceContext is not provided here.");
#endif
    return context_;
  }

public:
  virtual ~Service() = default;
  virtual void initialize() {}
  virtual void terminate() {}
  virtual void tick() {}
  virtual void update() {}
  virtual void render() {}
  virtual bool running() { return true; }
  virtual bool keep_alive() { return false; }
  virtual void exec(const std::vector<std::string>& args) {}
};

class ServiceContextProvider
{
public:
  virtual ~ServiceContextProvider() = default;
  [[nodiscard]] virtual util::Iterable<std::shared_ptr<Service>> services() const = 0;

private:
  void set_context(const ServiceContext* context) const
  {
    for (const auto& service : services())
      service->context_ = context;
  }

  friend class ServiceContext;
};

class ServiceContext
{
public:
  Axgl* axgl = nullptr;

private:
  ServiceContextProvider* provider_;

public:
  ServiceContext(ServiceContextProvider* provider) : provider_(provider)
  {
    provider_->set_context(this);
  }
  ServiceContext(ServiceContextProvider* provider, Axgl* axgl) : ServiceContext(provider)
  {
    this->axgl = axgl;
  }
  ServiceContext(ServiceContextProvider* provider, const ServiceContext* context) : ServiceContext(provider)
  {
    axgl = context->axgl;
  }
  ServiceContext(const ServiceContext&) = delete;
  ServiceContext& operator=(const ServiceContext&) = delete;
  ServiceContext(ServiceContext&&) = delete;
  ServiceContext& operator=(ServiceContext&&) = delete;

  ~ServiceContext()
  {
    provider_->set_context(nullptr);
  }
};

NAMESPACE_AXGL_INTERFACE_END
