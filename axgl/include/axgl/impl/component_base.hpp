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
  bool disabled_ = false;
  std::uint32_t ticks_ = 0;

protected:
  std::string id_;
  std::weak_ptr<Entity> entity_;

public:
  void set_id(const std::string& id) override { id_ = id; }
  [[nodiscard]] std::string get_id() const override { return id_; }

  void tick() override { ++ticks_; }
  [[nodiscard]] std::uint32_t ticks() const override { return ticks_; }
  void set_disabled(const bool disabled) override { disabled_ = disabled; }
  [[nodiscard]] bool is_disabled() const override { return disabled_; }

  void set_entity(std::weak_ptr<Entity> entity) override { entity_ = std::move(entity); }
  [[nodiscard]] std::shared_ptr<Entity> get_entity() const override
  {
#ifdef AXGL_DEBUG
    if (!entity_.expired())
      throw std::runtime_error("Component entity is not set.");
#endif
    return entity_.lock();
  }
};

} // namespace axgl::impl
