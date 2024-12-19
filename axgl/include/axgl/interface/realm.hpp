#pragma once

#include <format>
#include <stdint.h>

#include "axgl/except.hpp"
#include "axgl/namespace.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/interface/renderer.hpp"
#include "axgl/interface/camera.hpp"
#include "axgl/interface/light.hpp"
#include "axgl/util/iterable.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
  void use_context(const RealmContext* context)
  {
    for (const auto& component : get_components())
    {
      component->context_ = context;
      component->parent_ = this;
      component->use_context(context);
    }
  }
  void clear_context()
  {
    for (const auto& component : get_components())
    {
      component->context_ = nullptr;
      component->parent_ = nullptr;
      component->clear_context();
    }
  }

private:
  glm::mat4 model_matrix_{ 1.0f };

public:
  glm::vec3 scale{ 1.0f };
  glm::vec3 rotation{ 0.0f };
  glm::vec3 position{ 0.0f };

  virtual ~Component() {}
  virtual void update() {}
  virtual void render() {}

  void update_model_matrix()
  {
    model_matrix_ = glm::translate(glm::mat4(1.0f), position)
      * glm::toMat4(glm::quat(rotation))
      * glm::scale(scale);
  }

  glm::mat4 get_model() const
  {
    return parent_ ? parent_->model_matrix_ * model_matrix_ : model_matrix_;
  }

  uint32_t get_id() const
  {
    return id_;
  }

  virtual void add_component(std::shared_ptr<Component> component) {}
  virtual void remove_component(uint32_t id) {}

  virtual util::Iterable<std::shared_ptr<Component>> get_components() const = 0;

  friend class Realm;
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
  Component* provider_;
public:
  RealmContext(Component* provider) : provider_(provider)
  {
    provider_->use_context(this);
  }
  RealmContext(Component* provider, const RealmContext* context) : RealmContext(provider)
  {
    axgl = context->axgl;
    renderer = context->renderer;
    realm = context->realm;
    pv = context->pv;
  }
  RealmContext(const RealmContext&) = delete;
  RealmContext& operator=(const RealmContext&) = delete;
  RealmContext(RealmContext&&) = delete;
  RealmContext& operator=(RealmContext&&) = delete;
  ~RealmContext()
  {
    provider_->clear_context();
  }
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
};

NAMESPACE_AXGL_INTERFACE_END
