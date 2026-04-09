#pragma once

#include <axgl/interface/component.hpp>

#include <axgl/impl/context_holder.hpp>

namespace axgl::impl
{

class ComponentBase : virtual public axgl::Component, public axgl::impl::ContextHolder
{
  static std::uint64_t next_id()
  {
    static std::uint64_t id = 0;
    return ++id;
  }

protected:
  bool disabled_ = false;
  std::uint64_t id_ = next_id();

public:
  void set_disabled(const bool disabled) override { disabled_ = disabled; }
  [[nodiscard]] bool is_disabled() const override { return disabled_; }
  [[nodiscard]] std::uint64_t get_id() const override { return id_; }
};

} // namespace axgl::impl
