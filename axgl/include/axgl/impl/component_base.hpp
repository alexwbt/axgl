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
  bool static_ = false;
  bool hidden_ = false;
  std::uint64_t id_ = next_id();

public:
  void set_disabled(const bool disabled) override { disabled_ = disabled; }
  void set_static(const bool is_static) override { static_ = is_static; }
  void set_hidden(const bool hidden) override { hidden_ = hidden; }
  [[nodiscard]] bool is_disabled() const override { return disabled_; }
  [[nodiscard]] bool is_static() const override { return static_; }
  [[nodiscard]] bool is_hidden() const override { return hidden_; }
  [[nodiscard]] std::uint64_t get_id() const override { return id_; }
};

} // namespace axgl::impl
