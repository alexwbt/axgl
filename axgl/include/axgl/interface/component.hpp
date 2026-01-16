#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/entity.hpp>

namespace axgl
{

class Component
{
public:
  virtual ~Component() = default;
  virtual void tick(const axgl::Entity::Context& context) { }
  virtual void update(const axgl::Entity::Context& context) { }
  virtual void on_entity_create(const axgl::Entity::Context& context) { }
  virtual void on_entity_remove(const axgl::Entity::Context& context) { }

  virtual void set_name(const std::string& id) = 0;
  virtual void set_disabled(bool disabled) = 0;

  [[nodiscard]] virtual std::uint64_t get_id() const = 0;
  [[nodiscard]] virtual std::string get_name() const = 0;
  [[nodiscard]] virtual bool is_disabled() const = 0;
  [[nodiscard]] virtual std::uint64_t ticks() const = 0;
};

} // namespace axgl
