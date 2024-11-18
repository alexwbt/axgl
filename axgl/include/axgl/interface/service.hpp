#pragma once

#include <memory>

#include "axgl/namespace.hpp"

NAMESPACE_AXGL_INTERFACE

class Service
{
public:
  virtual ~Service() {}
  virtual void initialize() = 0;
  virtual void terminate() = 0;
  virtual void update() = 0;
  virtual void render() = 0;
  virtual bool running() = 0;
  virtual bool keep_alive() = 0;
};

class CommandService : public Service
{
public:
  virtual void exec(const std::vector<std::string>& args) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
