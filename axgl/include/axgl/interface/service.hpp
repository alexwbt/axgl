#pragma once

#include <memory>

#include "axgl/namespace.hpp"

NAMESPACE_AXGL_INTERFACE

struct ServiceContext;

class Service
{
public:
  virtual ~Service() {}
  virtual void initialize() = 0;
  virtual void terminate() = 0;
  virtual void update(const ServiceContext& context) = 0;
  virtual void render(const ServiceContext& context) = 0;
  virtual bool running() = 0;
  virtual bool keep_alive() = 0;
  virtual void exec(const std::vector<std::string>& args) {}
};

class Axgl;
struct ServiceContext
{
  Axgl* axgl;
};

NAMESPACE_AXGL_INTERFACE_END
