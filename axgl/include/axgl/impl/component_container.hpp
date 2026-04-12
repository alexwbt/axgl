#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/component.hpp>
#include <axgl/interface/container.hpp>

#include <axgl/impl/context_holder.hpp>

namespace axgl::impl
{

class ComponentContainer : virtual public axgl::Container<axgl::Component>, public axgl::impl::ContextHolder
{
  axgl::Entity* parent_;
  std::vector<axgl::ptr_t<axgl::Component>> components_;

public:
  explicit ComponentContainer(axgl::Entity* parent) : parent_(parent) { }

  void set_context(const axgl::Context* context) override
  {
    axgl::impl::ContextHolder::set_context(context);

    for (const auto& entity : components_)
      entity->set_context(context);
  }

  void tick() const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled() && !comp->is_static()) comp->parent_tick(parent_);
  }

  void update() const
  {
    for (const auto& comp : get())
      if (!comp->is_disabled() && !comp->is_static()) comp->parent_update(parent_);
  }

  void on_create() const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled() && !comp->is_static()) comp->on_parent_create(parent_);
  }

  void on_remove() const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled() && !comp->is_static()) comp->on_parent_remove(parent_);
  }

  void add(axgl::ptr_t<axgl::Component> component) override
  {
    component->set_context(context_);
    components_.push_back(std::move(component));
  }
  void remove(const axgl::ptr_t<axgl::Component>& component) override { std::erase(components_, component); }
  [[nodiscard]] std::span<const ptr_t<axgl::Component>> get() const override { return components_; }
  [[nodiscard]] std::uint64_t size() const override { return components_.size(); }
  [[nodiscard]] bool empty() const override { return components_.empty(); }
};

} // namespace axgl::impl
