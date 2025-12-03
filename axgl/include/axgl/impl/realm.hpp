#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>

#include <axgl/impl/entity_container.hpp>

namespace axgl::impl
{

class Realm : public axgl::Realm
{
  axgl::impl::EntityContainer entities_;

public:
  void tick(const Service::Context& context) override { entities_.tick({context, *this}); }
  void update(const Service::Context& context) override { entities_.update({context, *this}); }

  [[nodiscard]] EntityContainer& entities() override { return entities_; }
};

} // namespace axgl::impl
