#pragma once

#ifdef AXGL_DEBUG
#include <stdexcept>
#endif
#include <cstdint>

#include <axgl/interface/component.hpp>
#include <axgl/interface/entity.hpp>

namespace axgl::impl
{

class ComponentBase : virtual public axgl::Component
{
  std::string id_;
  std::uint64_t ticks_ = 0;
  bool disabled_ = false;

public:
  void tick(const Entity::Context& context) override { ++ticks_; }

  [[nodiscard]] std::uint64_t ticks() const override { return ticks_; }

  void set_id(const std::string& id) override { id_ = id; }
  [[nodiscard]] std::string get_id() const override { return id_; }

  void set_disabled(const bool disabled) override { disabled_ = disabled; }
  [[nodiscard]] bool is_disabled() const override { return disabled_; }
};

} // namespace axgl::impl
