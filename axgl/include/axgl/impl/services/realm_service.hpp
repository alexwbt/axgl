#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/services/realm_service.hpp>

#include <axgl/impl/realm.hpp>

namespace axgl::impl
{

class RealmService : virtual public axgl::RealmService
{
  axgl::ptr_t<axgl::Realm> realm_;

public:
  void tick(const Service::Context& context) override
  {
    if (!realm_) return;
    realm_->tick(context);
  }

  void update(const Service::Context& context) override
  {
    if (!realm_) return;
    realm_->update(context);
  }

  axgl::ptr_t<axgl::Realm> create_default_realm() override { return axgl::create_ptr<impl::Realm>(); }

  [[nodiscard]] axgl::ptr_t<axgl::Realm> get_active_realm() const override { return realm_; }

  void set_active_realm(axgl::ptr_t<axgl::Realm> realm) override { realm_ = std::move(realm); }
};

} // namespace axgl::impl
