#pragma once

#include <vector>

#include <axgl/common.hpp>
#include <axgl/interface/component.hpp>
#include <axgl/interface/container.hpp>

#include <axgl/util/iterable.hpp>

namespace axgl::impl
{

class ComponentContainer : virtual public axgl::Container<axgl::Component>
{
  std::vector<axgl::ptr_t<axgl::Component>> components_;

public:
  void tick(const axgl::Entity::Context& context) const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled())
        comp->tick(context);
  }

  void update(const axgl::Entity::Context& context) const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled())
        comp->update(context);
  }

  void on_entity_create(const axgl::Entity::Context& context) const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled())
        comp->on_entity_create(context);
  }

  void on_entity_remove(const axgl::Entity::Context& context) const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled())
        comp->on_entity_remove(context);
  }

  void add(axgl::ptr_t<axgl::Component> component) override { components_.push_back(std::move(component)); }
  void remove(const axgl::ptr_t<axgl::Component>& component) override { std::erase(components_, component); }
  [[nodiscard]] util::Iterable<axgl::ptr_t<axgl::Component>> get() const override
  {
    return util::to_iterable_t<axgl::ptr_t<axgl::Component>>(components_);
  }
};

} // namespace axgl::impl
