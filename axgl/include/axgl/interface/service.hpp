#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "axgl/namespace.hpp"

NAMESPACE_AXGL
class Axgl;
NAMESPACE_AXGL_END

NAMESPACE_AXGL_INTERFACE

class ServiceContext;
class ServiceContextHolder;
class Service
{
private:
  const ServiceContext* context_;
  friend class ServiceContextHolder;

protected:
  const ServiceContext* context() { return context_; }

public:
  virtual ~Service() {}
  virtual void initialize() = 0;
  virtual void terminate() = 0;
  virtual void update() = 0;
  virtual void render() = 0;
  virtual bool running() = 0;
  virtual bool keep_alive() = 0;
  virtual void exec(const std::vector<std::string>& args) {}
};

class ServiceContextHolder
{
public:
  virtual ~ServiceContextHolder() {}

private:
  virtual void apply_context(const ServiceContext* context) = 0;

protected:
  void set_context(Service& service, const ServiceContext* context)
  {
    service.context_ = context;
  }

  friend class ServiceContext;
};

class ServiceContext
{
public:
  Axgl* axgl;

private:
  ServiceContextHolder* holder_;

public:
  ServiceContext(ServiceContextHolder* holder) : holder_(holder)
  {
    holder_->apply_context(this);
  }
  ServiceContext(ServiceContextHolder* holder, const ServiceContext* context) : ServiceContext(holder)
  {
    axgl = context->axgl;
  }
  ServiceContext(const ServiceContext&) = delete;
  ServiceContext& operator=(const ServiceContext&) = delete;
  ServiceContext(ServiceContext&&) = delete;
  ServiceContext& operator=(ServiceContext&&) = delete;

  ~ServiceContext()
  {
    holder_->apply_context(nullptr);
  }
};

NAMESPACE_AXGL_INTERFACE_END
