#pragma once

#include <vector>

#include "axgl/axgl.hpp"
#include "axgl/namespace.hpp"
#include "axgl/interface/renderer.hpp"
#include "axgl/interface/realm.hpp"

NAMESPACE_AXGL_IMPL

class Entity : public interface::Entity
{
private:
  std::vector<std::shared_ptr<interface::Component>> components_;

public:
  void update() override
  {
    interface::RealmContext context(this, get_context());
    context.entity = this;

    for (const auto& comp : components_)
      comp->update();
  }

  void render() override
  {
    interface::RealmContext context(this, get_context());
    context.entity = this;
    context.model = model();

    for (const auto& comp : components_)
      comp->render();
  }

  void add_component(std::shared_ptr<interface::Component> component) override
  {
    components_.push_back(std::move(component));
  }

  util::Iterable<std::shared_ptr<interface::Component>> components() const override
  {
    return util::to_iterable_t<std::shared_ptr<interface::Component>>(components_);
  }
};

class Realm : public interface::Realm
{
private:
  std::vector<std::shared_ptr<Entity>> entities_;
  std::shared_ptr<interface::Renderer> renderer_;

public:
  void update() override
  {
    interface::RealmContext context(this);
    context.axgl = get_context()->axgl;
    context.renderer = renderer_.get();
    context.realm = this;

    for (const auto& entity : entities_)
      entity->update();
  }

  void render() override
  {
    if (!renderer_ || !renderer_->ready()) return;

    renderer_->before_render();

    interface::RealmContext context(this);
    context.axgl = get_context()->axgl;
    context.renderer = renderer_.get();
    context.realm = this;
    context.pv = camera.pv(renderer_->viewport());
    context.model = model();

    for (const auto& entity : entities_)
      entity->render();

    renderer_->after_render();
  }

  void set_renderer(std::shared_ptr<interface::Renderer> renderer) override
  {
    renderer_ = std::move(renderer);
  }

  util::Iterable<std::shared_ptr<interface::Entity>> entities() const override
  {
    return util::to_iterable_t<std::shared_ptr<interface::Entity>>(entities_);
  }

  std::shared_ptr<interface::Entity> create_entity() override
  {
    auto entity = std::make_shared<Entity>();
    entities_.push_back(entity);
    return entity;
  }
};

class RealmService : public interface::RealmService
{
private:
  std::shared_ptr<Realm> realm_;

public:
  void update() override
  {
    if (!realm_) return;

    interface::RealmContext context(this);
    context.axgl = get_context()->axgl;

    realm_->update();
  }

  void render() override
  {
    if (!realm_) return;

    interface::RealmContext context(this);
    context.axgl = get_context()->axgl;

    realm_->render();
  }

  std::shared_ptr<interface::Realm> create_realm() override
  {
    return realm_ = std::make_shared<Realm>();
  }

  std::shared_ptr<interface::Realm> get_active_realm() const override
  {
    return realm_;
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::RealmService> Axgl::use_service()
{
  auto realm_service = std::make_shared<impl::RealmService>();
  register_service("realm", realm_service);

  return realm_service;
}

NAMESPACE_AXGL_END
