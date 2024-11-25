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
  void update(const interface::RealmContext& context) override
  {
    auto current_context = context;
    current_context.entity = this;

    for (const auto& comp : components_)
      comp->update(current_context);
  }

  void render(const interface::RealmContext& context) override
  {
    auto current_context = context;
    current_context.entity = this;

    for (const auto& comp : components_)
      comp->render(current_context);
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
  void update(const interface::RealmContext& context) override
  {
    interface::RealmContext current_context;
    current_context.axgl = context.axgl;
    current_context.realm = this;
    current_context.renderer = renderer_.get();

    for (const auto& entity : entities_)
      entity->update(current_context);
  }

  void render(const interface::RealmContext& context) override
  {
    if (!renderer_) return;
    interface::RealmContext current_context;
    current_context.axgl = context.axgl;
    current_context.realm = this;
    current_context.renderer = renderer_.get();

    renderer_->before_render();

    for (const auto& entity : entities_)
      entity->render(current_context);

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
};

class DefaultRealmService : public interface::RealmService
{
private:
  std::shared_ptr<DefaultRealm> realm_;

public:
  void initialize() override {}
  void terminate() override {}

  void update(const interface::ServiceContext& context) override
  {
    if (realm_)
      realm_->update({ context.axgl });
  }

  void render(const interface::ServiceContext& context) override
  {
    if (realm_)
      realm_->render({ context.axgl });
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
