#pragma once

#include <memory>
#include <stdint.h>

#include <entt/entt.hpp>

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/renderer.hpp>
#include <axgl/impl/realm_service.hpp>

NAMESPACE_AXGL_IMPL

class EnttComponent : public interface::Component
{
private:
public:
};

class EnttRealm : public interface::Realm
{
private:
  std::shared_ptr<interface::Renderer> renderer_;

  entt::registry registry_;

public:
  void set_renderer(std::shared_ptr<interface::Renderer> renderer) override
  {
    renderer_ = std::move(renderer);
  }

  void update() override
  {
    auto view = registry_.view<ComponentPtr>();
    for (auto [entity, comp_ptr] : view.each())
    {
      comp_ptr.comp->update();
    }
  }

  void render() override
  {
    comp_impl_.render();
  }

  void add_component(std::shared_ptr<interface::Component> component) override
  {
    auto comp = std::dynamic_pointer_cast<EnttComponent>(component);
#ifdef AXGL_DEBUG
    if (!comp)
      throw std::runtime_error(
        "Invalid component type, "
        "components of EnttRealm must be EnttComponent.");
#endif
    comp_impl_.add_component(std::move(comp));
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

class EnttRealmService : public interface::RealmService
{
public:
  std::shared_ptr<EnttRealm> create_realm()
  {

  }

  std::shared_ptr<EnttRealm> get_active_realm() const
  {

  }

  void set_active_realm(std::shared_ptr<EnttRealm> realm)
  {

  }
};

NAMESPACE_AXGL_IMPL_END
