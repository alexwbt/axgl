#pragma once

#include <memory>
#include <vector>

#include <axgl/interface/realm.hpp>
#include <axgl/interface/services/realm_service.hpp>

#include <axgl/impl/realm.hpp>
#include <axgl/impl/service_base.hpp>

namespace axgl::impl
{

class RealmService : virtual public axgl::RealmService, public ServiceBase
{
  std::shared_ptr<Realm> realm_;
  std::vector<std::shared_ptr<Realm>> realms_;

  Realm::Context context_;

public:
  void initialize() override { context_.axgl = get_context()->axgl; }

  void tick() override
  {
    if (!realm_)
      return;
    realm_->tick();
  }

  void update() override
  {
    if (!realm_)
      return;

    context_.realm = realm_.get();
    realm_->update();
  }

  void render() override
  {
    if (!realm_)
      return;
    realm_->render();
  }

  std::shared_ptr<axgl::Realm> create_realm() override
  {
    realm_ = std::make_shared<Realm>();
    realm_->set_context(&context_);
    realms_.push_back(realm_);
    return realm_;
  }

  std::shared_ptr<axgl::Realm> get_active_realm() const override { return realm_; }

  void set_active_realm(std::shared_ptr<axgl::Realm> realm) override
  {
    auto realm_impl = std::dynamic_pointer_cast<Realm>(realm);
#ifdef AXGL_DEBUG
    if (!realm_impl)
      throw std::runtime_error("Failed to set active realm. "
                               "Default Realm is required for default RealmService");
#endif
    realm_ = std::move(realm_impl);
  }
};

} // namespace axgl::impl
