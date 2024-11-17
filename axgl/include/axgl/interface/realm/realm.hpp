#pragma once

#include "axgl/namespace.hpp"
#include "axgl/interface/renderer.hpp"
#include "axgl/interface/realm/entity.hpp"

NAMESPACE_AXGL_INTERFACE

class Realm : public Renderable
{
public:
  virtual ~Realm() {}
  virtual std::shared_ptr<Entity> create_entity() = 0;
};

class RealmService : public Service
{
public:
  virtual std::shared_ptr<Realm> create_realm() = 0;
};

NAMESPACE_AXGL_INTERFACE_END
