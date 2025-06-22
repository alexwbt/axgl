#pragma once

#include <vector>

#include <axgl/common.hpp>

NAMESPACE_AXGL
class Axgl;
NAMESPACE_AXGL_END

NAMESPACE_AXGL_INTERFACE

struct ServiceContext final
{
  Axgl* axgl = nullptr;
};

class Service
{
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

  virtual void set_context(const ServiceContext* context) = 0;
protected:
  virtual const ServiceContext* get_context() const = 0;
};

NAMESPACE_AXGL_INTERFACE_END
