#pragma once

#include <string>

#include <glm/glm.hpp>

#include <axgl/interface/component_manager.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/transform.hpp>

namespace axgl
{

class Component;

class Entity
{
public:
  virtual ~Entity() = default;
  virtual void tick() { }
  virtual void update() { }
  virtual void render() { }
  virtual void on_create() { }
  virtual void on_remove() { }

  virtual uint32_t ticks() const = 0;
  virtual void set_disabled(bool) = 0;
  virtual bool is_disabled() const = 0;

  virtual void set_id(const std::string& id) = 0;
  virtual std::string get_id() const = 0;

  virtual Transform* transform() = 0;
  virtual void update_model_matrix() = 0;
  virtual glm::mat4 get_model_matrix() const = 0;

  virtual void mark_remove(bool should_remove) = 0;
  virtual bool should_remove() const = 0;

  virtual ComponentManager* components() = 0;

  virtual void set_context(Realm::Context* context) = 0;
  virtual Realm::Context* get_context() = 0;
};

} // namespace axgl
