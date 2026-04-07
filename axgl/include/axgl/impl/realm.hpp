#pragma once

#include <axgl/interface/realm.hpp>

#include <axgl/impl/entity_container.hpp>

namespace axgl::impl
{

class Realm : public axgl::Realm
{
protected:
  axgl::impl::EntityContainer entities_{nullptr};

public:
  void tick() override { entities_.tick(); }
  void update() override { entities_.update(); }
  void initialize() override { }

  [[nodiscard]] axgl::impl::EntityContainer& entities() override { return entities_; }
};

} // namespace axgl::impl
