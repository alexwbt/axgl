#pragma once

#include <axgl/axgl.hpp>

/* Service implementations */

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::BundlefileResourceService> Axgl::use_service()
{
  auto resource_service = std::make_shared<impl::BundlefileResourceService>();
  register_service("resource", resource_service);

  return resource_service;
}

NAMESPACE_AXGL_END
