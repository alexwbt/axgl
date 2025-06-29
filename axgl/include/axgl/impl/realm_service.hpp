#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <axgl/axgl.hpp>
#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>

NAMESPACE_AXGL_IMPL

class ComponentContainer final
{
  std::vector<std::shared_ptr<interface::Component>> components_;

public:
  void tick() const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled())
        comp->tick();
  }

  void update() const
  {
    ZoneScopedN("Component Container Update");

    for (const auto& comp : components_)
      if (!comp->is_disabled())
        comp->update();
  }

  void render() const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled())
        comp->render();
  }

  void on_create() const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled())
        comp->on_create();
  }

  void on_remove() const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled())
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
    for (const auto& component : components_)
      component->set_context(context);
  }
};

class EntityContainer final
{
  std::vector<std::shared_ptr<interface::Entity>> entities_;

public:
  void tick() const
  {
    for (const auto& entity : entities_)
      entity->tick();
  }

  void update()
  {
    ZoneScopedN("Entity Container Update");

    for (const auto& entity : entities_)
    {
      if (entity->ticks() == 0)
        entity->on_create();

      entity->update();
    }


    if (!entities_.empty())
    {
      ZoneScopedN("Entity Container Remove Entities");

      std::erase_if(entities_,
        [](const auto& entity)
        {
          const auto should_remove = entity->should_remove();
          if (should_remove)
            entity->on_remove();
          return should_remove;
        });
    }
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
        e->mark_remove(true);
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

class ComponentBase : virtual public interface::Component
{
  bool disabled_ = false;
  uint32_t ticks_ = 0;

protected:
  std::string id_;
  interface::RealmContext* context_ = nullptr;
  interface::Entity* parent_ = nullptr;

  void set_id(const std::string& id) override { id_ = id; }
  [[nodiscard]] std::string get_id() const override { return id_; }

  [[nodiscard]] interface::RealmContext* get_context() const override
  {
#ifdef AXGL_DEBUG
    if (!context_)
      throw std::runtime_error(id_ + ": RealmContext is not provided.");
#endif
    return context_;
  }

public:
  [[nodiscard]] interface::Entity* get_parent() const override
  {
#ifdef AXGL_DEBUG
    if (!parent_)
      throw std::runtime_error(id_ + ": Parent is not assigned.");
#endif
    return parent_;
  }

  void set_context(interface::RealmContext* context) override { context_ = context; }
  void set_parent(interface::Entity* parent) override { parent_ = parent; }

  void set_disabled(const bool disabled) override { disabled_ = disabled; }
  [[nodiscard]] bool is_disabled() const override { return disabled_; }

  void tick() override { ++ticks_; }
  [[nodiscard]] uint32_t ticks() const override { return ticks_; }
};

class Entity : virtual public interface::Entity, public ComponentBase
{
  glm::mat4 model_matrix_{ 1.0f };
  interface::Transformation transform_;

  bool should_remove_ = false;

protected:
  ComponentContainer components_;
  EntityContainer children_;

public:
  void mark_remove(const bool should_remove) override { should_remove_ = should_remove; }
  [[nodiscard]] bool should_remove() const override { return should_remove_; }

  interface::Transformation* transform() override { return &transform_; }

  void update_model_matrix() override
  {
    model_matrix_
      = glm::translate(glm::mat4(1.0f), transform_.position)
        * glm::toMat4(glm::quat(transform_.rotation))
        * glm::scale(transform_.scale);
  }

  [[nodiscard]] glm::mat4 get_model_matrix() const override
  {
    return parent_
      ? parent_->get_model_matrix() * model_matrix_
      : model_matrix_;
  }

  void tick() override
  {
    ComponentBase::tick();
    components_.tick();
    children_.tick();
  }

  void update() override
  {
    ZoneScopedN("Entity Update");

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

  void add_component(std::shared_ptr<interface::Component> component) override
  {
    component->set_parent(this);
    component->set_context(context_);
    components_.add_component(std::move(component));
  }

  void remove_component(const std::shared_ptr<interface::Component>& component) override
  {
    components_.remove_component(component);
  }

  [[nodiscard]] util::Iterable<std::shared_ptr<interface::Component>> get_components() const override
  {
    return components_.get_components();
  }

  void add_child(std::shared_ptr<interface::Entity> entity) override
  {
    entity->set_parent(this);
    entity->set_context(context_);
    children_.add_entity(std::move(entity));
  }

  void remove_child(const std::shared_ptr<interface::Entity>& entity) override
  {
    children_.remove_entity(entity);
  }

  [[nodiscard]] util::Iterable<std::shared_ptr<interface::Entity>> get_children() const override
  {
    return children_.get_entities();
  }

  void set_context(interface::RealmContext* context) override
  {
    ComponentBase::set_context(context);
    components_.set_context(context);
    children_.set_context(context);
  }
};

class Realm : public interface::Realm
{
  EntityContainer entities_;

  interface::RealmContext* context_ = nullptr;
  std::shared_ptr<interface::Renderer> renderer_;

public:
  void set_renderer(std::shared_ptr<interface::Renderer> renderer) override { renderer_ = std::move(renderer); }
  [[nodiscard]] std::shared_ptr<interface::Renderer> get_renderer() const override { return renderer_; }

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

    if (!context_ || !context_->camera || !renderer_ || !renderer_->ready())
      return;

    renderer_->before_render();

    entities_.render();

    renderer_->after_render();
  }

  void add_entity(std::shared_ptr<interface::Entity> entity) override
  {
    entity->set_context(context_);
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

  void set_context(interface::RealmContext* context) override
  {
    context_ = context;
    for (const auto& entity : get_entities())
      entity->set_context(context);
  }
};

class RealmService : virtual public interface::RealmService, public ServiceBase
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
    context_.camera = nullptr;
    context_.lights.clear();
    realm_->update();
  }

  void render() override
  {
    if (!realm_) return;
    realm_->render();
  }

  std::shared_ptr<interface::Realm> create_realm() override
  {
    realm_ = std::make_shared<Realm>();
    realm_->set_context(&context_);
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
