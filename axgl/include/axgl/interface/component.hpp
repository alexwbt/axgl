#pragma once

#include <stdint.h>
#include <string>

namespace axgl
{

class Entity;

class Component
{
public:
  virtual ~Component() = default;

  virtual void set_id(const std::string& id) = 0;
  virtual std::string get_id() const = 0;

  virtual void tick() { }
  virtual void update() { }
  virtual void render() { }
  virtual void on_create() { }
  virtual void on_remove() { }

  virtual uint32_t ticks() const = 0;
  virtual void set_disabled(bool) = 0;
  virtual bool is_disabled() const = 0;

  virtual void set_entity(Entity* entity) = 0;
  virtual Entity* get_entity() const = 0;
};

} // namespace axgl
