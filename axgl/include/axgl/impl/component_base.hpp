#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/component.hpp>
#include <axgl/interface/entity.hpp>

namespace axgl::impl
{

class ComponentBase : virtual public axgl::Component
{
  std::uint64_t id_ = 0;
  std::string name_;
  std::uint64_t ticks_ = 0;
  bool disabled_ = false;

public:
  void tick(const Entity::Context& context) override { ++ticks_; }

  void set_name(const std::string& name) override { name_ = name; }
  void set_disabled(const bool disabled) override { disabled_ = disabled; }

  [[nodiscard]] std::uint64_t get_id() const override { return id_; }
  [[nodiscard]] std::string get_name() const override { return name_; }
  [[nodiscard]] bool is_disabled() const override { return disabled_; }
  [[nodiscard]] std::uint64_t ticks() const override { return ticks_; }
};

} // namespace axgl::impl
