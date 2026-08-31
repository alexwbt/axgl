#pragma once

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/services/realm_service.hpp>

#include <axgl/impl/realm.hpp>
#include <axgl/impl/service_base.hpp>

namespace axgl::impl
{

class RealmService : virtual public axgl::RealmService,
                     public axgl::impl::ServiceBase
{
  using RealmFactory = std::function<axgl::ptr_t<axgl::Realm>()>;
  using RealmFactories = std::unordered_map<std::string, RealmFactory>;

  RealmFactories realm_factories_;
  axgl::ptr_t<axgl::Realm> realm_;
  bool activated_ = false;

public:
  void tick() override
  {
    if (!realm_ || !activated_) return;
    realm_->tick();
  }

  void update() override
  {
    if (!realm_) return;

    if (!activated_)
    {
      if (realm_->ticks() == 0) realm_->on_create();
      realm_->on_active();
      activated_ = true;
    }
    realm_->update();
  }

  void register_realm_factory(
    const std::string& type, std::function<ptr_t<axgl::Realm>()> realm_factory
  ) override
  {
    realm_factories_.emplace(type, realm_factory);
  }

  axgl::ptr_t<axgl::Realm> create_realm() override
  {
    return with_context(axgl::create_ptr<impl::Realm>());
  }

  axgl::ptr_t<axgl::Realm> create_realm(const std::string& type) override
  {
#ifdef AXGL_DEBUG
    if (!realm_factories_.contains(type))
      throw std::runtime_error(
        std::format("Realm factory for '{}' not registered.", type)
      );
#endif
    return with_context(realm_factories_.at(type)());
  }

  [[nodiscard]] axgl::ptr_t<axgl::Realm> get_active_realm() const override
  {
    return realm_;
  }

  void set_active_realm(axgl::ptr_t<axgl::Realm> realm) override
  {
    realm_ = std::move(realm);
    realm_->set_context(context_);
    activated_ = false;
  }
};

} // namespace axgl::impl
