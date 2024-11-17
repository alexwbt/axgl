#pragma once

#include "axgl/axgl.hpp"
#include "axgl/namespace.hpp"
#include "axgl/interface/realm/realm.hpp"
#include "axgl/interface/component/component.hpp"

NAMESPACE_AXGL_IMPL

class DefaultMesh2D : public interface::Mesh2D
{
public:
  void set_data(const std::vector<interface::Vertex2D>& data) {}
  void set_indices(const std::vector<uint32_t>& indices) {}
};

class DefaultEntity : public interface::Entity
{
public:
  void render() const override
  {

  }

  void add_component(std::shared_ptr<interface::Component> component) override
  {

  }
};

class DefaultRealm : public interface::Realm
{
public:
  void render() const override
  {

  }

  std::shared_ptr<interface::Entity> create_entity() override
  {
    return std::make_shared<DefaultEntity>();
  }
};

class DefaultRealmService : public interface::RealmService
{
public:
  void initialize() override {}
  void terminate() override {}
  void update() override {}
  void render() override {}
  bool running() override { return false; }

  std::shared_ptr<interface::Realm> create_realm() override
  {
    return std::make_shared<DefaultRealm>();
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::DefaultRealmService> Axgl::use_service()
{
  auto realm_service = std::make_shared<impl::DefaultRealmService>();
  register_service("realm", realm_service);

  return realm_service;
}

NAMESPACE_AXGL_END
