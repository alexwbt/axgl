#pragma once

#include "axgl/namespace.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/interface/renderer.hpp"
#include "axgl/interface/camera.hpp"
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
class Renderer;
class Component;
class RealmContext;
class RealmService;

class RealmContextProvider
{
public:
  virtual ~RealmContextProvider() {}
private:
  virtual void use_context(const RealmContext* context) = 0;

  friend struct RealmContext;
};

class RealmContext final
{
public:
  const Axgl* axgl = nullptr;
  const Renderer* renderer = nullptr;
  Realm* realm = nullptr;
  Entity* entity = nullptr;
  glm::mat4 pv{ 1.0f };
  glm::mat4 model{ 1.0f };
private:
  RealmContextProvider* provider_;
public:
  RealmContext(RealmContextProvider* provider) : provider_(provider) { provider_->use_context(this); }
  RealmContext(RealmContextProvider* provider, const RealmContext* context) : RealmContext(provider)
  {
    axgl = context->axgl;
    renderer = context->renderer;
    realm = context->realm;
    entity = context->entity;
    pv = context->pv;
    model = context->model;
  }
  RealmContext(const RealmContext&) = delete;
  RealmContext& operator=(const RealmContext&) = delete;
  RealmContext(RealmContext&&) = delete;
  RealmContext& operator=(RealmContext&&) = delete;
  ~RealmContext() { provider_->use_context(nullptr); }
};

class Component
{
private:
  const RealmContext* context_;
protected:
  const RealmContext* get_context() const { return context_; }

public:
  glm::vec3 scale{ 1.0f };
  glm::vec3 rotation{ 0.0f };
  glm::vec3 position{ 0.0f };

  virtual ~Component() {}
  virtual void update() = 0;
  virtual void render() = 0;

  glm::mat4 model() const
  {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position)
      * glm::toMat4(glm::quat(rotation)) * glm::scale(scale);

    if (context_) return context_->model * model;
    return model;
  }

  friend class Realm;
  friend class Entity;
  friend class RealmService;
};

class Entity : public Component, public RealmContextProvider
{
public:
  virtual ~Entity() {}
  virtual void add_component(std::shared_ptr<Component> component) = 0;
  virtual util::Iterable<std::shared_ptr<Component>> components() const = 0;

private:
  void use_context(const RealmContext* context) override
  {
    for (const auto& component : components())
      component->context_ = context;
  }
};

class Realm : public Component, public RealmContextProvider
{
public:
  interface::Camera camera;

public:
  virtual ~Realm() {}
  virtual void set_renderer(std::shared_ptr<Renderer> renderer) = 0;

  virtual util::Iterable<std::shared_ptr<Entity>> entities() const = 0;
  virtual std::shared_ptr<Entity> create_entity() = 0;

private:
  void use_context(const RealmContext* context) override
  {
    for (const auto& entity : entities())
      entity->context_ = context;
  }
};

class RealmService : public Service, public RealmContextProvider
{
public:
  virtual std::shared_ptr<Realm> create_realm() = 0;
  virtual std::shared_ptr<Realm> get_active_realm() const = 0;

private:
  void use_context(const RealmContext* context) override
  {
    if (auto realm = get_active_realm())
      realm->context_ = context;
  }
};

NAMESPACE_AXGL_INTERFACE_END
