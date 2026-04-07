#pragma once

#include <axgl/interface/component.hpp>

#include <axgl/impl/context_holder.hpp>

namespace axgl::impl
{

class ComponentBase : virtual public axgl::Component, public axgl::impl::ContextHolder
{
protected:
  axgl::Entity* parent_ = nullptr;
  bool disabled_ = false;

public:
  void set_parent(axgl::Entity* parent) override { parent_ = parent; }
  void set_disabled(const bool disabled) override { disabled_ = disabled; }
  [[nodiscard]] bool is_disabled() const override { return disabled_; }
};

} // namespace axgl::impl
