#pragma once

#include <cstdint>
#include <string>

#include <axgl/interface/entity.hpp>

namespace axgl
{

class Component
{
public:
  virtual ~Component() = default;
  virtual void tick(const Entity::Context& context) { }
  virtual void update(const Entity::Context& context) { }
  virtual void render(const Entity::Context& context) { }
  virtual void on_entity_create(const Entity::Context& context) { }
  virtual void on_entity_remove(const Entity::Context& context) { }

  virtual void set_id(const std::string& id) = 0;
  [[nodiscard]] virtual std::string get_id() const = 0;

  [[nodiscard]] virtual std::uint64_t ticks() const = 0;

  virtual void set_disabled(bool) = 0;
  [[nodiscard]] virtual bool is_disabled() const = 0;
};

} // namespace axgl
