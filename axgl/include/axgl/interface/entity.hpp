#pragma once

#include <cstdint>
#include <string>

#include <glm/glm.hpp>

#include <axgl/interface/container.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/transform.hpp>

namespace axgl
{

class Component;

class Entity
{
public:
  struct Context : Realm::Context
  {
    Entity& entity;
  };

  virtual ~Entity() = default;
  virtual void tick(const Realm::Context& context) { }
  virtual void update(const Realm::Context& context) { }
  virtual void render(const Realm::Context& context) { }
  virtual void on_create(const Realm::Context& context) { }
  virtual void on_remove(const Realm::Context& context) { }

  [[nodiscard]] virtual std::uint64_t ticks() const = 0;

  virtual void set_disabled(bool) = 0;
  [[nodiscard]] virtual bool is_disabled() const = 0;

  virtual void set_id(const std::string& id) = 0;
  [[nodiscard]] virtual std::string get_id() const = 0;

  virtual Transform& transform() = 0;
  virtual void update_model_matrix() = 0;
  [[nodiscard]] virtual glm::mat4 get_model_matrix() const = 0;

  virtual void mark_remove(bool should_remove) = 0;
  [[nodiscard]] virtual bool should_remove() const = 0;

  virtual Container<Component>* components() = 0;
};

} // namespace axgl
