#pragma once

#include <memory>
#include <string_view>

#include <axgl/interface/realm.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class RealmService : virtual public Service
{
public:
  static constexpr std::string_view kTypeId = "service::realm";

  virtual std::shared_ptr<Realm> create_realm() = 0;

  virtual void set_active_realm(std::shared_ptr<Realm> realm) = 0;
  [[nodiscard]] virtual std::shared_ptr<Realm> get_active_realm() const = 0;
};

} // namespace axgl

