#pragma once

#include <vector>
#include <memory>

#include <axgl/axgl.hpp>
#include <axgl/common.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/interface/realm.hpp>

NAMESPACE_AXGL_IMPL

class Component : public interface::Component
{
private:
  std::vector<std::shared_ptr<interface::Component>> components_;

public:
  void update() override
  {
    for (const auto& comp : components_)
      comp->update();
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
    components_.erase(
      std::remove_if(
        components_.begin(),
        components_.end(),
        [id](const auto& c)
    {
      return c->get_id() == id;
    }
      ),
      components_.end()
    );
  }

  util::Iterable<std::shared_ptr<interface::Component>> get_components() const override
  {
    return util::to_iterable_t<std::shared_ptr<interface::Component>>(components_);
  }

  const std::vector<std::shared_ptr<interface::Component>>& get_components_vector() const
  {
    return components_;
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
    interface::RealmContext context(this);
    context.renderer = renderer_.get();
    context.realm = this;

    for (const auto& entity : comp_impl_.get_components_vector())
      entity->update();
  }

  void render() override
  {
    if (!renderer_ || !renderer_->ready()) return;

    renderer_->before_render();

    interface::RealmContext context(this);
    context.renderer = renderer_.get();
    context.realm = this;
    context.pv = camera.pv(renderer_->viewport());

    for (const auto& entity : comp_impl_.get_components_vector())
      entity->render();

    renderer_->after_render();
  }

  void set_renderer(std::shared_ptr<interface::Renderer> renderer) override
  {
    renderer_ = std::move(renderer);
  }

  void add_component(std::shared_ptr<interface::Component> component) override
  {
    auto impl_component = std::dynamic_pointer_cast<Component>(component);
#ifdef AXGL_DEBUG
    if (!impl_component)
      throw std::runtime_error("The provided component is not a valid Component instance.");
#endif
    if (impl_component)
      comp_impl_.add_component(std::move(impl_component));
  }

  void remove_component(uint32_t id) override
  {
    comp_impl_.remove_component(id);
  }

  util::Iterable<std::shared_ptr<interface::Component>> get_components() const override
  {
    return comp_impl_.get_components();
  }
};

class RealmService : public interface::RealmService, public interface::Component
{
private:
  std::shared_ptr<Realm> realm_;
  std::vector<std::shared_ptr<Realm>> realms_;

public:
  void update() override
  {
    if (!realm_) return;

    interface::RealmContext context(this);
    context.axgl = interface::RealmService::get_context()->axgl;

    realm_->update();
  }

  void render() override
  {
    if (!realm_) return;

    interface::RealmContext context(this);
    context.axgl = interface::RealmService::get_context()->axgl;

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

  util::Iterable<std::shared_ptr<interface::Component>> get_components() const override
  {
    return util::to_iterable_t<std::shared_ptr<interface::Component>>(realms_);
  }
};

NAMESPACE_AXGL_IMPL_END

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::Component> interface::RealmService::create_component()
{
  return std::make_shared<impl::Component>();
}

template<>
std::shared_ptr<impl::RealmService> Axgl::use_service()
{
  auto realm_service = std::make_shared<impl::RealmService>();
  register_service("realm", realm_service);

  return realm_service;
}

NAMESPACE_AXGL_END
