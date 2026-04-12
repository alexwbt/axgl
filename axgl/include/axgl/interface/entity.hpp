#pragma once

#include <axgl/interface/component.hpp>
#include <axgl/interface/container.hpp>
#include <axgl/interface/context.hpp>
#include <axgl/interface/transform.hpp>

namespace axgl
{

class Entity : virtual public axgl::Component, virtual public axgl::ContextHolder
{
public:
  virtual void tick() { }
  virtual void update() { }
  virtual void on_create() { }
  virtual void on_remove() { }

  virtual void set_name(const std::string& name) = 0;
  virtual void mark_remove(bool should_remove) = 0;

  [[nodiscard]] virtual std::uint64_t ticks() const = 0;
  [[nodiscard]] virtual std::string get_name() const = 0;
  [[nodiscard]] virtual bool should_remove() const = 0;

  [[nodiscard]] virtual axgl::Transform& transform() = 0;
  [[nodiscard]] virtual axgl::Container<axgl::Entity>& children() = 0;
  [[nodiscard]] virtual axgl::Container<axgl::Component>& components() = 0;

  void parent_tick(axgl::Entity* parent) override { tick(); }
  void parent_update(axgl::Entity* parent) override { update(); }
  void on_parent_create(axgl::Entity* parent) override { on_create(); }
  void on_parent_remove(axgl::Entity* parent) override { on_remove(); }

  // region convenience functions

  void set_position(glm::vec3 position)
  {
    transform().position = position;
    transform().update_matrix();
  }

  void set_scale(glm::vec3 scale)
  {
    transform().scale = scale;
    transform().update_matrix();
  }

  void set_rotation(glm::vec3 rotation)
  {
    transform().rotation = rotation;
    transform().update_matrix();
  }

  void set_pivot(glm::vec3 pivot)
  {
    transform().pivot = pivot;
    transform().update_matrix();
  }

  void add_child(const axgl::ptr_t<axgl::Entity>& child) { children().add(child); }

  void add_children(const std::span<const axgl::ptr_t<axgl::Entity>>& children) { this->children().add_all(children); }

  void add_component(const axgl::ptr_t<axgl::Component>& component) { components().add(component); }

  void add_components(const std::span<const axgl::ptr_t<axgl::Component>>& components)
  {
    this->components().add_all(components);
  }

  template <typename ItemType>
  axgl::ptr_t<ItemType> get_children_t()
  {
    return children().get_t<ItemType>();
  }

  template <typename ItemType>
  axgl::ptr_t<ItemType> get_component_t()
  {
    return components().get_t<ItemType>();
  }

  // endregion
};

} // namespace axgl
