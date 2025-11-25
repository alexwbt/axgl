#pragma once

#include <memory>

#include <axgl/util/iterable.hpp>

namespace axgl
{

class Component;

class ComponentManager
{
public:
  virtual ~ComponentManager() = default;

  virtual void add(std::shared_ptr<Component> component) = 0;

  virtual void remove(const std::shared_ptr<Component>& component) = 0;

  virtual util::Iterable<std::shared_ptr<Component>> get() const = 0;

  template <typename ComponentType>
  std::shared_ptr<ComponentType> get_t()
  {
    for (const auto& component : get())
      if (auto component_t = std::dynamic_pointer_cast<ComponentType>(component))
        return component_t;
    return nullptr;
  }
};

} // namespace axgl

