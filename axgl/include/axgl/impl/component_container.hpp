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
  axgl::Entity* entity_;
  std::vector<std::shared_ptr<axgl::Component>> components_;

public:
  explicit ComponentContainer(axgl::Entity* entity) : entity_(entity) { }

  void tick() const
  {
    for (const auto& comp : components_)
      if (!comp->is_disabled())
        comp->tick();
  }

  void update() const
  {
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

  void add(std::shared_ptr<axgl::Component> component) override
  {
    component->set_entity(entity_);
    components_.push_back(std::move(component));
  }

  void remove(const std::shared_ptr<axgl::Component>& component) override { std::erase(components_, component); }

  [[nodiscard]] util::Iterable<std::shared_ptr<axgl::Component>> get() const override
  {
    return util::to_iterable_t<std::shared_ptr<axgl::Component>>(components_);
  }
};

} // namespace axgl::impl
