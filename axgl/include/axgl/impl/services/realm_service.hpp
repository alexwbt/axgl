#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/services/realm_service.hpp>

#include <axgl/impl/realm.hpp>
#include <axgl/impl/service_base.hpp>

namespace axgl::impl
{

class RealmService : virtual public axgl::RealmService, public axgl::impl::ServiceBase
{
  axgl::ptr_t<axgl::Realm> realm_;

public:
  void tick() override
  {
    if (!realm_) return;
    realm_->tick();
  }

  void update() override
  {
    if (!realm_) return;
    realm_->update();
  }

  axgl::ptr_t<axgl::Realm> create_realm() override { return axgl::create_ptr<impl::Realm>(); }

  [[nodiscard]] axgl::ptr_t<axgl::Realm> get_active_realm() const override { return realm_; }

  void set_active_realm(axgl::ptr_t<axgl::Realm> realm) override
  {
    realm_ = std::move(realm);
    realm_->set_context(context_);
  }
};

} // namespace axgl::impl
