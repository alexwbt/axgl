#pragma once

#include <cstdint>
#include <format>

#include <glm/glm.hpp>

#include <axgl/common.hpp>
#include <axgl/interface/camera.hpp>
#include <axgl/interface/light.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/util/iterable.hpp>

NAMESPACE_AXGL
class Axgl;
NAMESPACE_AXGL_END

NAMESPACE_AXGL_INTERFACE

class Realm;
class Entity;
class RealmService;
struct RealmContext;

class Component
{
public:
  virtual ~Component() = default;
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

protected:
  virtual RealmContext* get_context() const = 0;

public:
  virtual void set_context(RealmContext* context) = 0;
  virtual void set_parent(Entity* parent) = 0;
  virtual Entity* get_parent() const = 0;
};

struct Transformation final
{
  glm::vec3 scale{1.0f};
  glm::vec3 rotation{0.0f};
  glm::vec3 position{0.0f};
};

class Entity : virtual public Component
{
public:
  virtual Transformation* transform() = 0;
  virtual void update_model_matrix() = 0;
  virtual glm::mat4 get_model_matrix() const = 0;

  virtual void mark_remove(bool should_remove) = 0;
  virtual bool should_remove() const = 0;

  //
  // Component management functions
  //

  virtual void add_component(std::shared_ptr<Component> component) = 0;
  virtual void remove_component(const std::shared_ptr<Component>& component) = 0;
  virtual util::Iterable<std::shared_ptr<Component>> get_components() const = 0;

  template <typename ComponentType> std::shared_ptr<ComponentType> get_component_t()
  {
    for (const auto& comp : get_components())
      if (auto comp_t = std::dynamic_pointer_cast<ComponentType>(comp))
        return comp_t;
    return nullptr;
  }

  //
  // Entity management functions
  //

  virtual void add_child(std::shared_ptr<Entity> entity) = 0;
  virtual void remove_child(const std::shared_ptr<Entity>& entity) = 0;
  virtual util::Iterable<std::shared_ptr<Entity>> get_children() const = 0;

  template <typename EntityType> std::shared_ptr<EntityType> get_child_t()
  {
    for (const auto& entity : get_children())
      if (auto entity_t = std::dynamic_pointer_cast<EntityType>(entity))
        return entity_t;
    return nullptr;
  }
};

class Realm
{
public:
  virtual ~Realm() = default;
  virtual void set_context(RealmContext* context) = 0;
  virtual void set_renderer(std::shared_ptr<Renderer> renderer) = 0;
  virtual std::shared_ptr<Renderer> get_renderer() const = 0;

  virtual void tick() = 0;
  virtual void update() = 0;
  virtual void render() = 0;

  //
  // Entity management functions
  //

  virtual void add_entity(std::shared_ptr<Entity> entity) = 0;
  virtual void remove_entity(std::shared_ptr<Entity> entity) = 0;
  virtual util::Iterable<std::shared_ptr<Entity>> get_entities() const = 0;
};

class RealmService : virtual public Service
{
public:
  virtual std::shared_ptr<Realm> create_realm() = 0;
  virtual void set_active_realm(std::shared_ptr<Realm> realm) = 0;
  virtual std::shared_ptr<Realm> get_active_realm() const = 0;

  //
  // Component factory functions
  //

  template <typename ComponentType> std::shared_ptr<ComponentType> create_component()
  {
#ifdef AXGL_DEBUG
    throw std::runtime_error(std::format("Component type '{}' is not supported.", typeid(ComponentType).name()));
#else
    return nullptr;
#endif
  }

  template <typename ComponentType, typename ComponentImplType>
  std::shared_ptr<ComponentImplType> create_component_impl()
  {
    auto comp = create_component<ComponentType>();
    auto comp_impl = std::dynamic_pointer_cast<ComponentImplType>(comp);
#ifdef AXGL_DEBUG
    if (!comp_impl)
      throw std::runtime_error(
        std::format("Component implementation '{}' is not supported.", typeid(ComponentType).name()));
#endif
    return comp_impl;
  }

  //
  // Entity factory function
  //

  template <typename EntityType> std::shared_ptr<EntityType> create_entity()
  {
#ifdef AXGL_DEBUG
    throw std::runtime_error(std::format("Entity type '{}' is not supported.", typeid(EntityType).name()));
#else
    return nullptr;
#endif
  }

  template <typename EntityType, typename EntityImplType> std::shared_ptr<EntityImplType> create_entity_impl()
  {
    auto entity = create_entity<EntityType>();
    auto entity_impl = std::dynamic_pointer_cast<EntityImplType>(entity);
#ifdef AXGL_DEBUG
    if (!entity_impl)
      throw std::runtime_error(std::format("Entity implementation '{}' is not supported.", typeid(EntityType).name()));
#endif
    return entity_impl;
  }
};

struct RealmContext final
{
  const Axgl* axgl = nullptr;
  Realm* realm = nullptr;

  const Camera* camera = nullptr;
  std::vector<const Light*> lights;
};

NAMESPACE_AXGL_INTERFACE_END
