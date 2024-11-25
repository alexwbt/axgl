#pragma once

#include "axgl/namespace.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/interface/renderer.hpp"

NAMESPACE_AXGL_INTERFACE

class Axgl;
class Realm;
class Entity;
class Renderer;
struct RealmContext
{
  Axgl* axgl;
  Realm* realm;
  Entity* entity;
  Renderer* renderer;
};

class Component
{
public:
  virtual ~Component() {}
  virtual void update(const RealmContext& context) = 0;
  virtual void render(const RealmContext& context) = 0;
};

class Entity : public Component
{
public:
  virtual ~Entity() {}
  virtual void add_component(std::shared_ptr<Component> component) = 0;
};

class Realm
{
public:
  virtual ~Realm() {}
  virtual void update(const RealmContext& context) = 0;
  virtual void render(const RealmContext& context) = 0;
  virtual std::shared_ptr<Entity> create_entity() = 0;
  virtual void set_renderer(std::shared_ptr<Renderer> renderer) = 0;
};

class RealmService : public Service
{
public:
  virtual std::shared_ptr<Realm> create_realm() = 0;
  virtual void set_active_realm(std::shared_ptr<Realm> realm) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
