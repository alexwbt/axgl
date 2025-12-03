#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/services/realm_service.hpp>

#include <axgl/impl/realm.hpp>

namespace axgl::impl
{

class RealmService : virtual public axgl::RealmService
{
  axgl::ptr_t<axgl::impl::Realm> realm_;

public:
  void tick(const Service::Context& context) override
  {
    if (!realm_)
      return;

    realm_->tick(context);
  }

  void update(const Service::Context& context) override
  {
    if (!realm_)
    {
#ifdef AXGL_DEBUG
      SPDLOG_WARN("Active realm not set.");
#endif
      return;
    }

    realm_->update(context);
  }

  axgl::ptr_t<axgl::Realm> create_realm() override { return axgl::create_ptr<impl::Realm>(); }

  [[nodiscard]] axgl::ptr_t<axgl::Realm> get_active_realm() const override { return realm_; }

  void set_active_realm(const axgl::ptr_t<axgl::Realm> realm) override
  {
    auto realm_impl = axgl::ptr_cast<axgl::impl::Realm>(realm);
#ifdef AXGL_DEBUG
    if (!realm_impl)
      throw std::runtime_error(
        "Failed to set active realm. "
        "Default Realm is required for default RealmService");
#endif
    realm_ = std::move(realm_impl);
  }
};

} // namespace axgl::impl
