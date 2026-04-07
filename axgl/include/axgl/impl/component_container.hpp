#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/component.hpp>
#include <axgl/interface/container.hpp>

#include <axgl/impl/context_holder.hpp>

namespace axgl::impl
{

class ComponentContainer : virtual public axgl::Container<axgl::Component>, public axgl::impl::ContextHolder
{
  axgl::Entity* const parent_;
  std::vector<axgl::ptr_t<axgl::Component>> components_;

public:
  explicit ComponentContainer(axgl::Entity* parent) : parent_(parent) { }

  void set_context(const Context* context) override
  {
    axgl::impl::ContextHolder::set_context(context);

    for (const auto& entity : components_)
      entity->set_context(context);
  }

  void tick() const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled()) comp->tick();
  }

  void update() const
  {
    for (const auto& comp : get())
      if (!comp->is_disabled()) comp->update();
  }

  void on_create() const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled()) comp->on_create();
  }

  void on_remove() const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled()) comp->on_remove();
  }

  void add(axgl::ptr_t<axgl::Component> component) override
  {
    component->set_parent(parent_);
    component->set_context(context_);
    components_.push_back(std::move(component));
  }
  void remove(const axgl::ptr_t<axgl::Component>& component) override { std::erase(components_, component); }
  [[nodiscard]] std::span<const ptr_t<axgl::Component>> get() const override { return components_; }
};

} // namespace axgl::impl
