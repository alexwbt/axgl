#pragma once

#include <span>

#include "axgl/namespace.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/interface/renderer.hpp"
#include "axgl/interface/camera.hpp"
#include "axgl/util/iterable.hpp"

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

class RealmContextHolder
{
public:
  virtual ~RealmContextHolder() {}
private:
  virtual void apply_context(const RealmContext* context) = 0;

  friend struct RealmContext;
};

class RealmContext final
{
public:
  const Axgl* axgl;
  const Renderer* renderer;
  Realm* realm;
  Camera* camera;
  Entity* entity;
  float scale = 1.0f;
  glm::vec3 rotation{ 0.0f };
  glm::vec3 position{ 0.0f };
private:
  RealmContextHolder* holder_;
public:
  RealmContext(RealmContextHolder* holder) : holder_(holder)
  {
    holder_->apply_context(this);
  }
  RealmContext(RealmContextHolder* holder, const RealmContext* context) : RealmContext(holder)
  {
    axgl = context->axgl;
    renderer = context->renderer;
    realm = context->realm;
    camera = context->camera;
    entity = context->entity;
    scale = context->scale;
    rotation = context->rotation;
    position = context->position;
  }
  RealmContext(const RealmContext&) = delete;
  RealmContext& operator=(const RealmContext&) = delete;
  RealmContext(RealmContext&&) = delete;
  RealmContext& operator=(RealmContext&&) = delete;
  ~RealmContext()
  {
    holder_->apply_context(nullptr);
  }
};

class Component
{
private:
  const RealmContext* context_;
protected:
  const RealmContext* get_context() const { return context_; }

public:
  float scale = 1.0f;
  glm::vec3 rotation{ 0.0f };
  glm::vec3 position{ 0.0f };

  virtual ~Component() {}
  virtual void update() = 0;
  virtual void render() = 0;

  float get_scale() const
  {
    if (context_) return context_->scale * scale;
    return scale;
  }

  glm::vec3 get_rotation() const
  {
    if (context_) return context_->rotation + rotation;
    return rotation;
  }

  glm::vec3 get_position() const
  {
    if (context_) return context_->position + position;
    return position;
  }

  friend class Realm;
  friend class Entity;
  friend class RealmService;
};

class Entity : public Component, public RealmContextHolder
{
public:
  virtual ~Entity() {}
  virtual void add_component(std::shared_ptr<Component> component) = 0;
  virtual util::Iterable<std::shared_ptr<Component>> components() const = 0;

private:
  void apply_context(const RealmContext* context) override
  {
    for (const auto& component : components())
      component->context_ = context;
  }
};

class Realm : public Component, public RealmContextHolder
{
public:
  virtual ~Realm() {}
  virtual void set_renderer(std::shared_ptr<Renderer> renderer) = 0;

  virtual util::Iterable<std::shared_ptr<Entity>> entities() const = 0;
  virtual std::shared_ptr<Entity> create_entity() = 0;

private:
  void apply_context(const RealmContext* context) override
  {
    for (const auto& entity : entities())
      entity->context_ = context;
  }
};

class RealmService : public Service, public RealmContextHolder
{
public:
  virtual std::shared_ptr<Realm> create_realm() = 0;
  virtual std::shared_ptr<Realm> get_active_realm() const = 0;

private:
  void apply_context(const RealmContext* context) override
  {
    if (auto realm = get_active_realm())
      realm->context_ = context;
  }
};

NAMESPACE_AXGL_INTERFACE_END
