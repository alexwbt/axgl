#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/container.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/transform.hpp>
#include <glm/glm.hpp>

namespace axgl
{

class Component;

class Entity
{
public:
  struct Context : axgl::Realm::Context
  {
    Entity& entity;
  };

  virtual ~Entity() = default;
  virtual void tick(const axgl::Realm::Context& context) { }
  virtual void update(const axgl::Realm::Context& context) { }
  virtual void on_create(const axgl::Realm::Context& context) { }
  virtual void on_remove(const axgl::Realm::Context& context) { }

  virtual void set_name(const std::string& id) = 0;
  virtual void set_disabled(bool) = 0;
  virtual void update_model_matrix() = 0;
  virtual void mark_remove(bool should_remove) = 0;

  [[nodiscard]] virtual std::uint64_t get_id() const = 0;
  [[nodiscard]] virtual std::string get_name() const = 0;
  [[nodiscard]] virtual bool is_disabled() const = 0;
  [[nodiscard]] virtual glm::mat4 get_model_matrix() const = 0;
  [[nodiscard]] virtual Transform& transform() = 0;
  [[nodiscard]] virtual std::uint64_t ticks() const = 0;
  [[nodiscard]] virtual bool should_remove() const = 0;

  [[nodiscard]] virtual axgl::Container<axgl::Component>& components() = 0;
};

} // namespace axgl
