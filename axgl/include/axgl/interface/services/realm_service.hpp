#pragma once

#include <memory>

#include <axgl/interface/realm.hpp>
#include <axgl/interface/service.hpp>

namespace axgl
{

class RealmService : virtual public Service
{
public:
  virtual std::shared_ptr<Realm> create_realm() = 0;

  virtual void set_active_realm(std::shared_ptr<Realm> realm) = 0;
  virtual std::shared_ptr<Realm> get_active_realm() const = 0;
};

} // namespace axgl

