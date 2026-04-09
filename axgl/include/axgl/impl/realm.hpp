#pragma once

#include <axgl/interface/realm.hpp>

#include <axgl/impl/context_holder.hpp>
#include <axgl/impl/entity_container.hpp>

namespace axgl::impl
{

class Realm : virtual public axgl::Realm, public axgl::impl::ContextHolder
{
protected:
  std::uint64_t ticks_ = 0;
  axgl::impl::EntityContainer entities_{nullptr};

public:
  void tick() override
  {
    ++ticks_;
    entities_.tick();
  }
  void update() override { entities_.update(); }
  void initialize() override { }

  [[nodiscard]] std::uint64_t ticks() override { return ticks_; }
  [[nodiscard]] axgl::impl::EntityContainer& entities() override { return entities_; }

  void set_context(const axgl::Context* context) override
  {
    axgl::impl::ContextHolder::set_context(context);
    entities_.set_context(context);
  }
};

} // namespace axgl::impl
