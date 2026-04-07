#pragma once

#include <axgl/interface/context.hpp>

namespace axgl
{

class Entity;

class Component : virtual public axgl::ContextHolder
{
public:
  virtual void tick() { }
  virtual void update() { }
  virtual void on_create() { }
  virtual void on_remove() { }

  virtual void set_parent(axgl::Entity* parent) = 0;
  virtual void set_disabled(bool disabled) = 0;
  [[nodiscard]] virtual bool is_disabled() const = 0;
};

} // namespace axgl
