#pragma once

#include <memory>
#include <vector>

#include <axgl/interface/component.hpp>
#include <axgl/interface/container.hpp>

#include <axgl/util/iterable.hpp>

namespace axgl::impl
{

class ComponentContainer : virtual public Container<axgl::Component>
{
  std::vector<std::shared_ptr<axgl::Component>> components_;

public:
  void tick(const Entity::Context& context) const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled())
        comp->tick(context);
  }

  void update(const Entity::Context& context) const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled())
        comp->update(context);
  }

  void render(const Entity::Context& context) const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled())
        comp->render(context);
  }

  void on_entity_create(const Entity::Context& context) const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled())
        comp->on_entity_create(context);
  }

  void on_entity_remove(const Entity::Context& context) const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled())
        comp->on_entity_remove(context);
  }

  void add(std::shared_ptr<axgl::Component> component) override { components_.push_back(std::move(component)); }
  void remove(const std::shared_ptr<axgl::Component>& component) override { std::erase(components_, component); }
  [[nodiscard]] util::Iterable<std::shared_ptr<axgl::Component>> get() const override
  {
    return util::to_iterable_t<std::shared_ptr<axgl::Component>>(components_);
  }
};

} // namespace axgl::impl
