#pragma once

#include "axgl/namespace.hpp"
#include "axgl/interface/service.hpp"
#include "axgl/interface/component/entity.hpp"

NAMESPACE_AXGL_INTERFACE

class Realm : public Component
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
