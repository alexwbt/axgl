#pragma once

#include <axgl/interface/entity.hpp>

#include <axgl/impl/component_base.hpp>
#include <axgl/impl/component_container.hpp>
#include <axgl/impl/entity_container.hpp>

namespace axgl::impl
{

class EntityBase : virtual public Entity, public axgl::impl::ComponentBase
{
protected:
  std::string name_;
  std::uint64_t ticks_ = 0;
  bool should_remove_ = false;
  axgl::Transform transform_;

  axgl::impl::ComponentContainer components_{this};
  axgl::impl::EntityContainer children_{this};

public:
  void set_context(const axgl::Context* context) override
  {
    axgl::impl::ComponentBase::set_context(context);
    components_.set_context(context);
    children_.set_context(context);
  }

  void tick() override
  {
    ++ticks_;
    components_.tick();
  }
  void update() override { components_.update(); }
  void on_create() override { components_.on_create(); }
  void on_remove() override { components_.on_remove(); }

  [[nodiscard]] std::uint64_t ticks() const override { return ticks_; }

  void set_name(const std::string& name) override { name_ = name; }
  void mark_remove(const bool should_remove) override { should_remove_ = should_remove; }

  [[nodiscard]] std::string get_name() const override { return name_; }
  [[nodiscard]] bool should_remove() const override { return should_remove_; }

  [[nodiscard]] axgl::Transform& transform() override { return transform_; }
  [[nodiscard]] axgl::impl::ComponentContainer& components() override { return components_; }
  [[nodiscard]] axgl::impl::EntityContainer& children() override { return children_; }
};

} // namespace axgl::impl
