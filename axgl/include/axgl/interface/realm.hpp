#pragma once

#include <format>
#include <stdint.h>

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
private:
  glm::mat4 model_matrix_{ 1.0f };

public:
  glm::vec3 scale{ 1.0f };
  glm::vec3 rotation{ 0.0f };
  glm::vec3 position{ 0.0f };

  uint32_t tick = 0;
  bool should_remove = false;

  virtual ~Component() {}
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

  uint32_t get_id() const
  {
    return id_;
  }

  virtual void add_component(std::shared_ptr<Component> component) {}
  virtual void remove_component(uint32_t id) {}

  virtual util::Iterable<std::shared_ptr<Component>> get_components() const = 0;

private:
  static uint32_t next_id()
  {
    static uint32_t next_id_ = 1;
    return next_id_++;
  }
  const RealmContext* context_ = nullptr;
  const Component* parent_ = nullptr;
  const uint32_t id_ = next_id();
protected:
  const RealmContext* get_context() const
  {
#ifdef AXGL_DEBUG
    if (!context_)
      throw std::runtime_error("RealmContext is not provided here.");
#endif
    return context_;
  }
  void use_context(const RealmContext* context) const
  {
    for (const auto& component : get_components())
    {
      component->context_ = context;
      component->parent_ = this;
      component->use_context(context);
    }
  }
  void clear_context() const
  {
    for (const auto& component : get_components())
    {
      component->context_ = nullptr;
      component->parent_ = nullptr;
      component->clear_context();
    }
  }

  friend class Realm;
  friend class RealmContext;
  friend class RealmService;
};

class Realm : public Component
{
public:
  interface::Camera camera;
  std::vector<interface::Light> lights;

public:
  virtual ~Realm() {}
  virtual void set_renderer(std::shared_ptr<Renderer> renderer) = 0;

  friend class RealmService;
};

class RealmService : public Service
{
public:
  virtual std::shared_ptr<Realm> create_realm() = 0;
  virtual std::shared_ptr<Realm> get_active_realm() const = 0;
  virtual void set_active_realm(std::shared_ptr<Realm> realm) = 0;

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

private:
  void use_context(RealmContext* context) const
  {
    if (auto realm = get_active_realm())
      realm->context_ = context;
  }
  void clear_context() const
  {
    if (auto realm = get_active_realm())
      realm->context_ = nullptr;
  }

  friend class RealmContext;
};

class RealmContext final
{
public:
  const Axgl* axgl = nullptr;
  const Renderer* renderer = nullptr;
  Realm* realm = nullptr;
  glm::mat4 pv{ 1.0f };
private:
  Component* provider_ = nullptr;
  RealmService* realm_service_ = nullptr;
public:
  RealmContext(Component* provider) : provider_(provider)
  {
#ifdef AXGL_DEBUG
    if (!provider)
      throw std::runtime_error("RealmContext initialized with nullptr provider.");
#endif
    provider_->use_context(this);
  }
  RealmContext(RealmService* realm_service) : realm_service_(realm_service)
  {
#ifdef AXGL_DEBUG
    if (!realm_service)
      throw std::runtime_error("RealmContext initialized with nullptr realm_service.");
#endif
    realm_service_->use_context(this);
  }
  RealmContext(const RealmContext&) = delete;
  RealmContext& operator=(const RealmContext&) = delete;
  RealmContext(RealmContext&&) = delete;
  RealmContext& operator=(RealmContext&&) = delete;
  ~RealmContext()
  {
    if (provider_)
      provider_->clear_context();
    else if (realm_service_)
      realm_service_->clear_context();
  }
  void copy(const RealmContext* context)
  {
    axgl = context->axgl;
    renderer = context->renderer;
    realm = context->realm;
    pv = context->pv;
  }
};

NAMESPACE_AXGL_INTERFACE_END
