#pragma once

#include "axgl/namespace.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/interface/renderer.hpp"
#include "axgl/interface/component/component.hpp"

NAMESPACE_AXGL_INTERFACE

// class Camera
// {
// public:
//   virtual ~Camera() {}
//   virtual void yaw(float yaw) = 0;
// };

class Entity
{
public:
  virtual ~Entity() {}
  virtual void update() = 0;
  virtual void render() const = 0;
  virtual void add_component(std::shared_ptr<Component> component) = 0;
};

class Realm
{
public:
  virtual ~Realm() {}
  virtual void update() = 0;
  virtual void render() const = 0;
  virtual void set_renderer(std::shared_ptr<Renderer> renderer) = 0;
  virtual std::shared_ptr<Entity> create_entity() = 0;
};

class RealmService : public Service
{
public:
  virtual std::shared_ptr<Realm> create_realm() = 0;
  virtual void set_active_realm(std::shared_ptr<Realm> realm) = 0;
};

NAMESPACE_AXGL_INTERFACE_END
