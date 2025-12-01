#pragma once

#include <memory>

#include <axgl/interface/realm.hpp>
#include <axgl/interface/services/realm_service.hpp>

#include <axgl/impl/realm.hpp>

namespace axgl::impl
{

class RealmService : virtual public axgl::RealmService
{
  std::shared_ptr<impl::Realm> realm_;

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
      return;

    realm_->update(context);
  }

  std::shared_ptr<axgl::Realm> create_realm() override { return std::make_shared<impl::Realm>(); }

  [[nodiscard]] std::shared_ptr<axgl::Realm> get_active_realm() const override { return realm_; }

  void set_active_realm(const std::shared_ptr<axgl::Realm> realm) override
  {
    auto realm_impl = std::dynamic_pointer_cast<impl::Realm>(realm);
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
