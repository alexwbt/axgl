#pragma once

#include <memory>

#include "axgl/namespace.hpp"
#include "axgl/interface/component/component.hpp"

NAMESPACE_AXGL_INTERFACE

class Entity : public Component
{
public:
  virtual ~Entity() {}
  virtual void add_component(std::shared_ptr<Component> component) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
