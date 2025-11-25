#pragma once

#include <cstdint>

#include <axgl/interface/component.hpp>
#include <axgl/interface/entity.hpp>

namespace axgl::impl
{

class ComponentBase : virtual public Component
{
  bool disabled_ = false;
  std::uint32_t ticks_ = 0;

protected:
  std::string id_;
  Entity* entity_ = nullptr;

public:
  void set_id(const std::string& id) override { id_ = id; }
  std::string get_id() const override { return id_; }

  void tick() override { ++ticks_; }
  uint32_t ticks() const override { return ticks_; }
  void set_disabled(const bool disabled) override { disabled_ = disabled; }
  bool is_disabled() const override { return disabled_; }

  void set_entity(Entity* entity) override { entity_ = entity; }
  Entity* get_entity() const override { return entity_; }
};

} // namespace axgl::impl
