#pragma once

#include <axgl/interface/component.hpp>
#include <axgl/interface/container.hpp>
#include <axgl/interface/context.hpp>

namespace axgl
{

class Entity : virtual public axgl::Component, virtual public axgl::ContextHolder
{
public:
  virtual void set_name(const std::string& name) = 0;
  virtual void mark_remove(bool should_remove) = 0;

  [[nodiscard]] virtual std::uint64_t get_id() const = 0;
  [[nodiscard]] virtual std::uint64_t ticks() const = 0;
  [[nodiscard]] virtual std::string get_name() const = 0;
  [[nodiscard]] virtual bool should_remove() const = 0;

  [[nodiscard]] virtual axgl::Container<axgl::Entity>& children() = 0;
  [[nodiscard]] virtual axgl::Container<axgl::Component>& components() = 0;

  template <typename ItemType>
  axgl::ptr_t<ItemType> get_component_t()
  {
    return components().get_t<ItemType>();
  }

  template <typename ItemType>
  axgl::ptr_t<ItemType> get_children_t()
  {
    return children().get_t<ItemType>();
  }
};

} // namespace axgl
