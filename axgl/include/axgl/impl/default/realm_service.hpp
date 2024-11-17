#pragma once

#include <vector>

#include "axgl/axgl.hpp"
#include "axgl/namespace.hpp"
#include "axgl/interface/realm/realm.hpp"
#include "axgl/interface/component/component.hpp"

NAMESPACE_AXGL_IMPL

class DefaultEntity : public interface::Entity
{
private:
  std::vector<std::shared_ptr<interface::Component>> components_;

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
private:
  std::vector<std::shared_ptr<DefaultEntity>> entities_;

public:
  void render() const override
  {
    for (const auto& entity : entities_)
      entity->render();
  }

  std::shared_ptr<interface::Entity> create_entity() override
  {
    auto entity = std::make_shared<DefaultEntity>();
    entities_.push_back(entity);
    return entity;
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
