#pragma once

#include <vector>
#include <memory>

#include <axgl/axgl.hpp>
#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>

#define AXGL_USE_COMPONENT_CONTAINER_IMPL(component_container) \
  public: \
    void add_component(std::shared_ptr<interface::Component> component) override \
    { \
      component->set_parent(this); \
      component->set_context(context_); \
      component_container.add_component(std::move(component)); \
    } \
    void remove_component(std::shared_ptr<interface::Component> component) override \
    { \
      component_container.remove_component(std::move(component)); \
    } \
    util::Iterable<std::shared_ptr<interface::Component>> get_components() const override \
    { \
      return component_container.get_components(); \
    }

#define AXGL_USE_ENTITY_CONTAINER_IMPL(entity_container) \
  public: \
    void add_child(std::shared_ptr<interface::Entity> entity) override \
    { \
      entity->set_parent(this); \
      entity->set_context(context_); \
      entity_container.add_entity(std::move(entity)); \
    } \
    void remove_child(std::shared_ptr<interface::Entity> entity) override \
    { \
      entity_container.remove_entity(std::move(entity)); \
    } \
    util::Iterable<std::shared_ptr<interface::Entity>> get_children() const override \
    { \
      return entity_container.get_entities(); \
    }

NAMESPACE_AXGL_IMPL

class ComponentContainer
{
  std::vector<std::shared_ptr<interface::Component>> components_;

public:
  void tick()
  {
    for (const auto& comp : components_)
      if (!comp->disabled)
        comp->tick();
  }

  void update()
  {
    for (const auto& comp : components_)
      if (!comp->disabled)
        comp->update();
  }

  void render()
  {
    for (const auto& comp : components_)
      if (!comp->disabled)
        comp->render();
  }

  void on_create()
  {
    for (const auto& comp : components_)
      if (!comp->disabled)
        comp->on_create();
  }

  void on_remove()
  {
    for (const auto& comp : components_)
      if (!comp->disabled)
        comp->on_remove();
  }

  void add_component(std::shared_ptr<interface::Component> component)
  {
    components_.push_back(std::move(component));
  }

  void remove_component(const std::shared_ptr<interface::Component>& component)
  {
    std::erase(components_, component);
  }

  [[nodiscard]] util::Iterable<std::shared_ptr<interface::Component>> get_components() const
  {
    return util::to_iterable_t<std::shared_ptr<interface::Component>>(components_);
  }

  void set_context(interface::RealmContext* context) const
  {
    for (const auto& comp : components_)
      comp->set_context(context);
  }

  void set_parent(interface::Entity* parent) const
  {
    for (const auto& comp : components_)
      comp->set_parent(parent);
  }
};

class EntityContainer
{
  std::vector<std::shared_ptr<interface::Entity>> entities_;

public:
  void tick() const
  {
    for (const auto& entity : entities_)
    {
      entity->tick();
      ++entity->ticks;
    }
  }

  void update()
  {
    for (const auto& entity : entities_)
    {
      if (entity->ticks == 0)
        entity->on_create();

      entity->update();
    }

    std::erase_if(entities_,
      [](const auto& entity)
      {
        if (entity->should_remove)
          entity->on_remove();
        return entity->should_remove;
      });
  }

  void render() const
  {
    for (const auto& entity : entities_)
      entity->render();
  }

  void on_create() const
  {
    for (const auto& entity : entities_)
      entity->on_create();
  }

  void on_remove() const
  {
    for (const auto& entity : entities_)
      entity->on_remove();
  }

  void add_entity(std::shared_ptr<interface::Entity> entity)
  {
    entities_.push_back(std::move(entity));
  }

  void remove_entity(const std::shared_ptr<interface::Entity>& entity) const
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

  [[nodiscard]] util::Iterable<std::shared_ptr<interface::Entity>> get_entities() const
  {
    return util::to_iterable_t<std::shared_ptr<interface::Entity>>(entities_);
  }

  void set_context(interface::RealmContext* context) const
  {
    for (const auto& entity : entities_)
      entity->set_context(context);
  }

  void set_parent(interface::Entity* parent) const
  {
    for (const auto& entity : entities_)
      entity->set_parent(parent);
  }
};

class Entity final : public interface::Entity
{
  ComponentContainer components_;
  EntityContainer children_;

public:
  void tick() override
  {
    components_.tick();
    children_.tick();
  }

  void update() override
  {
    components_.update();
    children_.update();
  }

  void render() override
  {
    components_.render();
    children_.render();
  }

  void on_create() override
  {
    components_.on_create();
    children_.on_create();
  }

  void on_remove() override
  {
    components_.on_remove();
    children_.on_remove();
  }

  AXGL_USE_COMPONENT_CONTAINER_IMPL(components_);
  AXGL_USE_ENTITY_CONTAINER_IMPL(children_);
};

class Realm final : public interface::Realm
{
  EntityContainer entities_;

public:
  void tick() override
  {
    entities_.tick();
  }

  void update() override
  {
    ZoneScopedN("Realm Update");

    entities_.update();
  }

  void render() override
  {
    ZoneScopedN("Realm Render");

    if (!context_
      || !context_->camera
      || !renderer_
      || !renderer_->ready()) return;

    renderer_->before_render();

    entities_.render();

    renderer_->after_render();
  }

  void add_entity(std::shared_ptr<interface::Entity> entity) override
  {
    entities_.add_entity(std::move(entity));
  }

  void remove_entity(const std::shared_ptr<interface::Entity> entity) override
  {
    entities_.remove_entity(entity);
  }

  [[nodiscard]] util::Iterable<std::shared_ptr<interface::Entity>> get_entities() const override
  {
    return entities_.get_entities();
  }
};

class RealmService final : public interface::RealmService
{
  std::shared_ptr<Realm> realm_;
  std::vector<std::shared_ptr<Realm>> realms_;

  interface::RealmContext context_;

public:
  void initialize() override
  {
    context_.axgl = get_context()->axgl;
  }

  void tick() override
  {
    if (!realm_) return;

    realm_->tick();
  }

  void update() override
  {
    if (!realm_) return;

    context_.realm = realm_.get();
    set_context(&context_);

    realm_->update();
  }

  void render() override
  {
    if (!realm_) return;

    realm_->render();
    set_context(nullptr);
    context_.camera = nullptr;
    context_.lights.clear();
  }

  std::shared_ptr<interface::Realm> create_realm() override
  {
    realm_ = std::make_shared<Realm>();
    realms_.push_back(realm_);
    return realm_;
  }

  [[nodiscard]] std::shared_ptr<interface::Realm> get_active_realm() const override
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
inline std::shared_ptr<interface::Entity> interface::RealmService::create_entity()
{
  return std::make_shared<impl::Entity>();
}
#endif

template<>
inline std::shared_ptr<impl::Entity> interface::RealmService::create_entity()
{
  return std::make_shared<impl::Entity>();
}

template<>
inline std::shared_ptr<impl::RealmService> Axgl::use_service()
{
  auto realm_service = std::make_shared<impl::RealmService>();
  register_service(DefaultServices::kRealm, realm_service);

  return realm_service;
}

NAMESPACE_AXGL_END
