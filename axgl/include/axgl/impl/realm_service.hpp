#pragma once

#include <vector>
#include <memory>

#include <axgl/axgl.hpp>
#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/renderer.hpp>

NAMESPACE_AXGL_IMPL

class Component : public interface::Component
{
private:
  std::vector<std::shared_ptr<interface::Component>> components_;

public:
  void update() override
  {
    for (const auto& comp : components_)
    {
      if (comp->tick == 0)
        comp->on_create();

      comp->update();
      ++comp->tick;
    }

    components_.erase(
      std::remove_if(components_.begin(), components_.end(),
        [](const auto& c)
        {
          if (c->should_remove)
            c->on_remove();
          return c->should_remove;
        }),
      components_.end()
    );
  }

  void render() override
  {
    for (const auto& comp : components_)
      comp->render();
  }

  void add_component(std::shared_ptr<interface::Component> component) override
  {
    components_.push_back(std::move(component));
  }

  void remove_component(uint32_t id) override
  {
    for (const auto& comp : components_)
    {
      if (comp->get_id() == id)
      {
        comp->should_remove = true;
        break;
      }
    }
  }

  util::Iterable<std::shared_ptr<interface::Component>> get_components() const override
  {
    return util::to_iterable_t<std::shared_ptr<interface::Component>>(components_);
  }

  std::shared_ptr<interface::Component> get_component(uint32_t id) const override
  {
    for (const auto& comp : components_)
      if (comp->get_id() == id)
        return comp;
    return nullptr;
  }
};

class Realm : public interface::Realm
{
private:
  impl::Component comp_impl_;
  std::shared_ptr<interface::Renderer> renderer_;

public:
  void update() override
  {
    ZoneScopedN("Realm Update");

    interface::RealmContext context(this);
    context.axgl = get_context()->axgl;
    context.renderer = renderer_.get();
    context.realm = this;

    comp_impl_.update();
  }

  void render() override
  {
    ZoneScopedN("Realm Render");

    if (!renderer_ || !renderer_->ready()) return;

    renderer_->before_render();

    interface::RealmContext context(this);
    context.axgl = get_context()->axgl;
    context.renderer = renderer_.get();
    context.realm = this;
    context.pv = camera.pv(renderer_->viewport());

    comp_impl_.render();

    renderer_->after_render();
  }

  void set_renderer(std::shared_ptr<interface::Renderer> renderer) override
  {
    renderer_ = std::move(renderer);
  }

  void add_component(std::shared_ptr<interface::Component> component) override
  {
    comp_impl_.add_component(std::move(component));
  }

  void remove_component(uint32_t id) override
  {
    comp_impl_.remove_component(id);
  }

  util::Iterable<std::shared_ptr<interface::Component>> get_components() const override
  {
    return comp_impl_.get_components();
  }

  std::shared_ptr<interface::Component> get_component(uint32_t id) const override
  {
    return comp_impl_.get_component(id);
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

#ifndef AXGL_DEFINED_CREATE_COMPONENT
#define AXGL_DEFINED_CREATE_COMPONENT
template<>
std::shared_ptr<interface::Component> interface::RealmService::create_component()
{
  return std::make_shared<impl::Component>();
}
#endif

template<>
std::shared_ptr<impl::RealmService> Axgl::use_service()
{
  auto realm_service = std::make_shared<impl::RealmService>();
  register_service("realm", realm_service);

  return realm_service;
}

NAMESPACE_AXGL_END
