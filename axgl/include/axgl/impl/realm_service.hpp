#pragma once

#include <vector>
#include <memory>

#include <axgl/axgl.hpp>
#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>

NAMESPACE_AXGL_IMPL

class EntityContainer
{
private:
  std::vector<std::shared_ptr<interface::Entity>> entities_;

public:
  void update()
  {
    for (const auto& entity : entities_)
    {
      if (entity->tick == 0)
        entity->on_create();

      entity->update();
      ++entity->tick;
    }

    entities_.erase(
      std::remove_if(entities_.begin(), entities_.end(),
        [](const auto& entity)
        {
          if (entity->should_remove)
            entity->on_remove();
          return entity->should_remove;
        }),
      entities_.end()
    );
  }

  void render()
  {
    for (const auto& entity : entities_)
      entity->render();
  }

  void on_create()
  {
    for (const auto& entity : entities_)
      entity->on_create();
  }

  void on_remove()
  {
    for (const auto& entity : entities_)
      entity->on_remove();
  }

  void add_entity(std::shared_ptr<interface::Entity> entity)
  {
    entities_.push_back(std::move(entity));
  }

  void remove_entity(std::shared_ptr<interface::Entity> entity)
  {
    for (auto& e : entities_)
    {
      if (e == entity)
      {
        e->should_remove = true;
        break;
      }
    }
  }
  
  util::Iterable<std::shared_ptr<interface::Entity>> get_entities()
  {
    return util::to_iterable_t<std::shared_ptr<interface::Entity>>(entities_);
  }
};

class Entity : public interface::Entity
{
private:
  std::vector<std::shared_ptr<interface::Component>> components_;

  EntityContainer children_;

public:
  void update() override
  {
    for (const auto& comp : components_)
      comp->update();

    children_.update();
  }

  void render() override
  {
    for (const auto& comp : components_)
      comp->render();

    children_.render();
  }

  void on_create() override
  {
    for (const auto& comp : components_)
      comp->on_create();

    children_.on_create();
  }

  void on_remove() override
  {
    for (const auto& comp : components_)
      comp->on_remove();

    children_.on_remove();
  }

  void add_component(std::shared_ptr<Component> component)
  {
    components_.push_back(std::move(component));
  }

  void remove_component(std::shared_ptr<Component> component)
  {
    components_.erase(
      std::remove(components_.begin(), components_.end(), component),
      components_.end()
    );
  }

  util::Iterable<std::shared_ptr<interface::Component>> get_components()
  {
    return util::to_iterable_t<std::shared_ptr<interface::Component>>(components_);
  }

  void add_child(std::shared_ptr<Entity> entity)
  {
    children_.add_entity(std::move(entity));
  }

  void remove_child(std::shared_ptr<Entity> entity)
  {
    children_.remove_entity(std::move(entity));
  }

  util::Iterable<std::shared_ptr<interface::Entity>> get_children()
  {
    return children_.get_entities();
  }
};

class Realm : public interface::Realm
{
private:
  EntityContainer entities_;

public:
  void update() override
  {
    ZoneScopedN("Realm Update");
    
    entities_.update();
  }

  void render() override
  {
    ZoneScopedN("Realm Render");

    if (!renderer_ || !renderer_->ready()) return;

    renderer_->before_render();

    entities_.render();

    renderer_->after_render();
  }

  util::Iterable<std::shared_ptr<interface::Entity>> get_entities()
  {
  }
};

class RealmService : public interface::RealmService
{
private:
  std::shared_ptr<Realm> realm_;
  std::vector<std::shared_ptr<Realm>> realms_;

public:
  void update() override
  {
    if (!realm_) return;

    interface::RealmContext context(this);
    context.axgl = get_context()->axgl;
    context.realm = realm_.get();
    realm_->update();
  }

  void render() override
  {
    if (!realm_) return;

    interface::RealmContext context(this);
    context.axgl = get_context()->axgl;
    context.realm = realm_.get();
    realm_->render();
  }

  std::shared_ptr<interface::Realm> create_realm() override
  {
    realm_ = std::make_shared<Realm>();
    realms_.push_back(realm_);
    return realm_;
  }

  std::shared_ptr<interface::Realm> get_active_realm() const override
  {
    return realm_;
  }

  void set_active_realm(std::shared_ptr<interface::Realm> realm) override
  {
    auto realm_impl = std::dynamic_pointer_cast<Realm>(realm);
#ifdef AXGL_DEBUG
    if (!realm_impl)
      throw std::runtime_error(
        "Failed to set active realm. "
        "Default Realm is required for default RealmService");
#endif
    realm_ = std::move(realm_impl);
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

#ifndef AXGL_DEFINED_CREATE_ENTITY
#define AXGL_DEFINED_CREATE_ENTITY
template<>
std::shared_ptr<interface::Entity> interface::RealmService::create_entity()
{
  return std::make_shared<impl::Entity>();
}
#endif

template<>
std::shared_ptr<impl::RealmService> Axgl::use_service()
{
  auto realm_service = std::make_shared<impl::RealmService>();
  register_service(DefaultServices::kRealm, realm_service);

  return realm_service;
}

NAMESPACE_AXGL_END
