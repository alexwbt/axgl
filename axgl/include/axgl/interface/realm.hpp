#pragma once

#include <format>
#include <cstdint>

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/camera.hpp>
#include <axgl/interface/light.hpp>
#include <axgl/util/iterable.hpp>

NAMESPACE_AXGL
class Axgl;
NAMESPACE_AXGL_END

NAMESPACE_AXGL_INTERFACE

class Realm;
class Entity;
class RealmService;
class RealmContext;

class Component
{
public:
  bool disabled = false;

protected:
  RealmContext* context_ = nullptr;
  Entity* parent_ = nullptr;

public:
  virtual ~Component() = default;
  virtual void tick() {}
  virtual void update() {}
  virtual void render() {}
  virtual void on_create() {}
  virtual void on_remove() {}

  [[nodiscard]] Entity* get_parent() const
  {
#ifdef AXGL_DEBUG
    if (!parent_)
      throw std::runtime_error("Parent is not assigned.");
#endif
    return parent_;
  }

protected:
  [[nodiscard]] RealmContext* get_context() const
  {
#ifdef AXGL_DEBUG
    if (!context_)
      throw std::runtime_error("RealmContext is not provided.");
#endif
    return context_;
  }

public:
  virtual void set_context(RealmContext* context)
  {
    context_ = context;
  }

  virtual void set_parent(Entity* parent)
  {
    parent_ = parent;
  }
};

class Entity : public Component
{
  glm::mat4 model_matrix_{ 1.0f };

public:
  glm::vec3 scale{ 1.0f };
  glm::vec3 rotation{ 0.0f };
  glm::vec3 position{ 0.0f };

  uint32_t ticks = 0;
  bool should_remove = false;

  void update_model_matrix()
  {
    model_matrix_
      = glm::translate(glm::mat4(1.0f), position)
      * glm::toMat4(glm::quat(rotation))
      * glm::scale(scale);
  }

  [[nodiscard]] glm::mat4 get_model() const
  {
    return parent_
      ? parent_->model_matrix_ * model_matrix_
      : model_matrix_;
  }

  //
  // Component management functions
  //

  virtual void add_component(std::shared_ptr<Component> component) = 0;
  virtual void remove_component(std::shared_ptr<Component> component) = 0;
  [[nodiscard]] virtual util::Iterable<std::shared_ptr<Component>> get_components() const = 0;

  template<typename ComponentType>
  [[nodiscard]] std::shared_ptr<ComponentType> get_component_t()
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
  virtual void remove_child(std::shared_ptr<Entity> entity) = 0;
  [[nodiscard]] virtual util::Iterable<std::shared_ptr<Entity>> get_children() const = 0;

  template<typename EntityType>
  [[nodiscard]] std::shared_ptr<EntityType> get_child_t()
  {
    for (const auto& entity : get_children())
      if (auto entity_t = std::dynamic_pointer_cast<EntityType>(entity))
        return entity_t;
    return nullptr;
  }

  //
  // Context
  //

  void set_context(RealmContext* context) override
  {
    // set context
    Component::set_context(context);
    // set context for all components
    for (const auto& component : get_components())
      component->set_context(context);
    // set context for all children
    for (const auto& child : get_children())
      child->set_context(context);
  }
};

class Realm
{
protected:
  RealmContext* context_ = nullptr;
  std::shared_ptr<Renderer> renderer_;

public:
  virtual ~Realm() = default;

  void set_renderer(std::shared_ptr<Renderer> renderer) { renderer_ = std::move(renderer); }
  [[nodiscard]] std::shared_ptr<Renderer> get_renderer() const { return renderer_; }

  virtual void tick() = 0;
  virtual void update() = 0;
  virtual void render() = 0;

  //
  // Entity management functions
  //

  virtual void add_entity(std::shared_ptr<Entity> entity) = 0;
  virtual void remove_entity(std::shared_ptr<Entity> entity) = 0;
  [[nodiscard]] virtual util::Iterable<std::shared_ptr<Entity>> get_entities() const = 0;

  //
  // Context
  //

  virtual void set_context(RealmContext* context)
  {
    context_ = context;
    for (const auto& entity : get_entities())
      entity->set_context(context);
  }
};

class RealmService : public Service
{
public:
  virtual std::shared_ptr<Realm> create_realm() = 0;
  virtual void set_active_realm(std::shared_ptr<Realm> realm) = 0;
  [[nodiscard]] virtual std::shared_ptr<Realm> get_active_realm() const = 0;

  //
  // Component factory functions
  //

  template<typename ComponentType>
  std::shared_ptr<ComponentType> create_component()
  {
#ifdef AXGL_DEBUG
    throw std::runtime_error(
      std::format("Component type '{}' is not supported.",
        typeid(ComponentType).name()));
#else
    return nullptr;
#endif
  }

  template<typename ComponentType, typename ComponentImplType>
  std::shared_ptr<ComponentImplType> create_component_impl()
  {
    auto comp = create_component<ComponentType>();
    auto comp_impl = std::dynamic_pointer_cast<ComponentImplType>(comp);
#ifdef AXGL_DEBUG
    if (!comp_impl)
      throw std::runtime_error(
        std::format("Component implementation '{}' is not supported.",
          typeid(ComponentType).name()));
#endif
    return comp_impl;
  }

  //
  // Entity factory function
  //

  template<typename EntityType>
  std::shared_ptr<EntityType> create_entity()
  {
#ifdef AXGL_DEBUG
    throw std::runtime_error(
      std::format("Entity type '{}' is not supported.",
        typeid(EntityType).name()));
#else
    return nullptr;
#endif
  }

  template<typename EntityType, typename EntityImplType>
  std::shared_ptr<EntityImplType> create_entity_impl()
  {
    auto entity = create_entity<EntityType>();
    auto entity_impl = std::dynamic_pointer_cast<EntityImplType>(entity);
#ifdef AXGL_DEBUG
    if (!entity_impl)
      throw std::runtime_error(
        std::format("Entity implementation '{}' is not supported.",
          typeid(EntityType).name()));
#endif
    return entity_impl;
  }

  //
  // Context
  //

protected:
  virtual void set_context(RealmContext* context)
  {
    if (auto realm = get_active_realm())
      realm->set_context(context);
  }
};

class RealmContext final
{
public:
  const Axgl* axgl = nullptr;
  Realm* realm = nullptr;

  const Camera* camera = nullptr;
  std::vector<const Light*> lights;
};

NAMESPACE_AXGL_INTERFACE_END
