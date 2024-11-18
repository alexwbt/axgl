#pragma once

#include "axgl/namespace.hpp"

NAMESPACE_AXGL_INTERFACE

class Component
{
public:
  virtual ~Component() {}
  virtual void update() = 0;
  virtual void render() const = 0;

  template <typename ComponentType>
  static std::shared_ptr<ComponentType> create_component()
  {
    throw std::runtime_error(
      std::format("Component type '{}' is not supported.",
        typeid(ComponentType).name()));
  }
};

NAMESPACE_AXGL_INTERFACE_END
