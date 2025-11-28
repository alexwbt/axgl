#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace axgl
{

class Entity;

class Component
{
public:
  virtual ~Component() = default;

  virtual void set_id(const std::string& id) = 0;
  [[nodiscard]] virtual std::string get_id() const = 0;

  virtual void tick() { }
  virtual void update() { }
  virtual void render() { }
  virtual void on_create() { }
  virtual void on_remove() { }

  [[nodiscard]] virtual std::uint32_t ticks() const = 0;
  virtual void set_disabled(bool) = 0;
  [[nodiscard]] virtual bool is_disabled() const = 0;

  virtual void set_entity(std::weak_ptr<Entity> entity) = 0;
  [[nodiscard]] virtual std::shared_ptr<Entity> get_entity() const = 0;
};

} // namespace axgl
