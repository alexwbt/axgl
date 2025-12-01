#pragma once

#include <string_view>

#include <axgl/common.hpp>
#include <axgl/interface/realm.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class RealmService : virtual public axgl::Service
{
public:
  static constexpr std::string_view kTypeId = "service::realm";

  virtual axgl::ptr_t<axgl::Realm> create_realm() = 0;

  virtual void set_active_realm(axgl::ptr_t<axgl::Realm> realm) = 0;
  [[nodiscard]] virtual axgl::ptr_t<axgl::Realm> get_active_realm() const = 0;
};

} // namespace axgl

