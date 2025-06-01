#pragma once

#include <format>
#include <stdint.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <axgl/common.hpp>
#include <axgl/interface/service.hpp>
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
  friend class Entity;

private:
  RealmContext* context_ = nullptr;

protected:
  Entity* parent_ = nullptr;

public:
  uint32_t tick = 0;
  bool should_remove = false;

  virtual ~Component() {}
  virtual void update() {}
  virtual void render() {}
  virtual void on_create() {}
  virtual void on_remove() {}

protected:
  const RealmContext* get_context() const
  {
#ifdef AXGL_DEBUG
    if (!context_)
      throw std::runtime_error("RealmContext is not provided here.");
#endif
    return context_;
  }
};

class Entity : public Component
{
  friend class Realm;

private:
  glm::mat4 model_matrix_{ 1.0f };

public:
  glm::vec3 scale{ 1.0f };
  glm::vec3 rotation{ 0.0f };
  glm::vec3 position{ 0.0f };

public:
  virtual ~Entity() {}
  virtual void update() {}
  virtual void render() {}
  virtual void on_create() {}
  virtual void on_remove() {}

  void update_model_matrix()
  {
    model_matrix_
      = glm::translate(glm::mat4(1.0f), position)
      * glm::toMat4(glm::quat(rotation))
      * glm::scale(scale);
  }

  glm::mat4 get_model() const
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
  virtual util::Iterable<std::shared_ptr<interface::Component>> get_components() = 0;

  template<typename ComponentType>
  util::Iterable<std::shared_ptr<ComponentType>> get_components()
  {
#ifdef AXGL_DEBUG
    throw std::runtime_error(
      std::format("Component type '{}' is not supported.",
        typeid(ComponentType).name()));
#else
    return nullptr;
#endif
  }

  //
  // Entity management functions
  //

  virtual void add_child(std::shared_ptr<Entity> entity) = 0;
  virtual void remove_child(std::shared_ptr<Entity> entity) = 0;
  virtual util::Iterable<std::shared_ptr<interface::Entity>> get_children() = 0;

  //
  // Context
  //

private:
  virtual void use_context(RealmContext* context)
  {
    // set context
    context_ = context;
    // set context for all components
    for (const auto& component : get_components())
      component->context_ = context;
    // set context for all children
    for (const auto& child : get_children())
      child->use_context(context);
  }
};

class Realm
{
  friend class RealmService;

public:
  virtual ~Realm() {}

  //
  // Entity management functions
  //

  virtual void add_entity(std::shared_ptr<Entity> entity) = 0;
  virtual void remove_entity(std::shared_ptr<Entity> entity) = 0;
  virtual util::Iterable<std::shared_ptr<interface::Entity>> get_entities() = 0;

  //
  // Context
  //

private:
  virtual void use_context(RealmContext* context)
  {
    for (const auto& entity : get_entities())
      entity->use_context(context);
  }
};

class RealmService : public Service
{
  friend class RealmContext;

public:
  virtual std::shared_ptr<Realm> create_realm() = 0;
  virtual std::shared_ptr<Realm> get_active_realm() const = 0;
  virtual void set_active_realm(std::shared_ptr<Realm> realm) = 0;

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
  // Context
  //

private:
  virtual void use_context(RealmContext* context)
  {
    if (auto realm = get_active_realm())
      realm->use_context(context);
  }
};

class RealmContext final
{
public:
  const Axgl* axgl = nullptr;
  Realm* realm = nullptr;
  glm::mat4 pv{ 1.0f };

private:
  RealmService* realm_service_;

public:
  RealmContext(RealmService* realm_service) : realm_service_(realm_service)
  {
#ifdef AXGL_DEBUG
    if (!realm)
      throw std::runtime_error("RealmContext initialized with nullptr realm service.");
#endif
    realm_service_->use_context(this);
  }
  RealmContext(const RealmContext&) = delete;
  RealmContext& operator=(const RealmContext&) = delete;
  RealmContext(RealmContext&&) = delete;
  RealmContext& operator=(RealmContext&&) = delete;
  ~RealmContext()
  {
    if (realm_service_)
      realm_service_->use_context(nullptr);
  }
};

NAMESPACE_AXGL_INTERFACE_END
