#pragma once

#include <span>

#include "axgl/namespace.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/interface/renderer.hpp"

NAMESPACE_AXGL
class Axgl;
NAMESPACE_AXGL_END

NAMESPACE_AXGL_INTERFACE

class RealmContext;
class RealmContextHolder;

class Component
{
private:
  const RealmContext* context_;
protected:
  const RealmContext* context() { return context_; }

public:
  virtual ~Component() {}
  virtual void update() = 0;
  virtual void render() = 0;

  friend class RealmContextHolder;
};

class RealmContextHolder
{
public:
  virtual ~RealmContextHolder() {}
private:
  virtual void apply_context(const RealmContext* context) = 0;

protected:
  void set_context(Component& comp, const RealmContext* context)
  {
    comp.context_ = context;
  }

  friend struct RealmContext;
};

class Realm;
class Entity;
class Renderer;
class RealmContext final
{
public:
  const Axgl* axgl;
  const Renderer* renderer;
  Realm* realm;
  Entity* entity;
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
    entity = context->entity;
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

class Entity : public Component, public RealmContextHolder
{
public:
  virtual ~Entity() {}
  virtual void add_component(std::shared_ptr<Component> component) = 0;
};

class Realm : public Component, public RealmContextHolder
{
public:
  virtual ~Realm() {}
  virtual std::shared_ptr<Entity> create_entity() = 0;
  virtual void set_renderer(std::shared_ptr<Renderer> renderer) = 0;
};

class RealmService : public Service, public RealmContextHolder
{
public:
  virtual std::shared_ptr<Realm> create_realm() = 0;
};

NAMESPACE_AXGL_INTERFACE_END
