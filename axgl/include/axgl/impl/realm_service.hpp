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
    interface::RealmContext current_context(this, context());
    current_context.entity = this;

    for (const auto& comp : components_)
      comp->update();
  }

  void render() override
  {
    interface::RealmContext current_context(this, context());
    current_context.entity = this;

    for (const auto& comp : components_)
      comp->render();
  }

  void add_component(std::shared_ptr<interface::Component> component) override
  {
    components_.push_back(std::move(component));
  }

  void apply_context(const interface::RealmContext* context) override
  {
    for (const auto& comp : components_)
      set_context(*comp, context);
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
    interface::RealmContext current_context(this);
    current_context.axgl = context()->axgl;
    current_context.realm = this;
    current_context.renderer = renderer_.get();

    for (const auto& entity : entities_)
      entity->update();
  }

  void render() override
  {
    if (!renderer_) return;
    interface::RealmContext current_context(this);
    current_context.axgl = context()->axgl;
    current_context.realm = this;
    current_context.renderer = renderer_.get();

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

  void apply_context(const interface::RealmContext* context) override
  {
    for (const auto& entity : entities_)
      set_context(*entity, context);
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
    if (!realm_) return;

    interface::RealmContext current_context(this);
    current_context.axgl = context()->axgl;

    realm_->update();
  }

  void render() override
  {
    if (!realm_) return;

    interface::RealmContext current_context(this);
    current_context.axgl = context()->axgl;

    realm_->render();
  }

  bool running() override { return true; }
  bool keep_alive() override { return false; }

  std::shared_ptr<interface::Realm> create_realm() override
  {
    return realm_ = std::make_shared<DefaultRealm>();
  }

  void apply_context(const interface::RealmContext* context) override
  {
    set_context(*realm_, context);
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
