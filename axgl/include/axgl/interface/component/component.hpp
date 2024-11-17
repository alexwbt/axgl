#pragma once

#include "axgl/namespace.hpp"

NAMESPACE_AXGL_INTERFACE

class Component
{
public:
  virtual ~Component() {}
  virtual void update() = 0;
  virtual void render() const = 0;
};

NAMESPACE_AXGL_INTERFACE_END
