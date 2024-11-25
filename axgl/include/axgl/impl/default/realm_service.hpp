#pragma once

#include <vector>

#include "axgl/axgl.hpp"
#include "axgl/namespace.hpp"
#include "axgl/interface/renderer.hpp"
#include "axgl/interface/realm.hpp"

NAMESPACE_AXGL_IMPL

class DefaultEntity : public interface::Entity
{
private:
  std::vector<std::shared_ptr<interface::Component>> components_;

public:
  void update() override
  {
    for (const auto& comp : components_)
      comp->update();
  }

  void render() const override
  {
    for (const auto& comp : components_)
      comp->render();
  }

  void add_component(std::shared_ptr<interface::Component> component) override
  {
    components_.push_back(std::move(component));
  }
};

class DefaultRealm : public interface::Realm
{
private:
  std::vector<std::shared_ptr<DefaultEntity>> entities_;
  std::shared_ptr<interface::Renderer> renderer_;

public:
  void update() override
  {
    for (const auto& entity : entities_)
      entity->update();
  }

  void render() const override
  {
    if (!renderer_) return;

    renderer_->before_render();

    for (const auto& entity : entities_)
      entity->render();

    renderer_->after_render();
  }

  std::shared_ptr<interface::Entity> create_entity() override
  {
    auto entity = std::make_shared<DefaultEntity>();
    entities_.push_back(entity);
    return entity;
  }

  void set_renderer(std::shared_ptr<interface::Renderer> renderer) override
  {
    renderer_ = std::move(renderer);
  }

  std::shared_ptr<interface::Renderer> get_renderer() const override
  {
    return renderer_;
  }
};

class DefaultRealmService : public interface::RealmService
{
private:
  std::shared_ptr<DefaultRealm> realm_;

public:
  void initialize() override {}
  void terminate() override {}

  void update() override
  {
    if (realm_)
      realm_->update();
  }

  void render() override
  {
    if (realm_)
      realm_->render();
  }

  bool running() override { return true; }
  bool keep_alive() override { return false; }

  std::shared_ptr<interface::Realm> create_realm() override
  {
    return realm_ = std::make_shared<DefaultRealm>();
  }

  void set_active_realm(std::shared_ptr<interface::Realm> realm) override
  {
    realm_ = dynamic_pointer_cast<DefaultRealm>(std::move(realm));
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
