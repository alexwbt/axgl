#pragma once

#include <axgl/interface/context.hpp>

namespace axgl
{

class Entity;

class Component : virtual public axgl::ContextHolder
{
public:
  virtual void parent_tick(axgl::Entity* parent) { }
  virtual void parent_update(axgl::Entity* parent) { }
  virtual void on_parent_create(axgl::Entity* parent) { }
  virtual void on_parent_remove(axgl::Entity* parent) { }

  virtual void set_disabled(bool disabled) = 0;
  virtual void set_static(bool is_static) = 0;
  virtual void set_hidden(bool hidden) = 0;

  [[nodiscard]] virtual bool is_disabled() const = 0;
  [[nodiscard]] virtual bool is_static() const = 0;
  [[nodiscard]] virtual bool is_hidden() const = 0;
  [[nodiscard]] virtual std::uint64_t get_id() const = 0;
};

} // namespace axgl
