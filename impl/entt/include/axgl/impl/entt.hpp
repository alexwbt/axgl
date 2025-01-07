#pragma once

#include <axgl/axgl.hpp>

#include <axgl/impl/entt/realm.hpp>

/* Service implementations */

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::EnttRealmService> Axgl::use_service()
{
  auto realm_service = std::make_shared<impl::EnttRealmService>();
  register_service("realm", realm_service);

  return realm_service;
}

NAMESPACE_AXGL_END
