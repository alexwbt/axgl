#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/services/bundlefile_service.hpp>

namespace axgl
{

template <>
inline std::shared_ptr<impl::bundlefile::BundlefileService> Axgl::use_service()
{
  auto service = std::make_shared<impl::bundlefile::BundlefileService>();
  register_service("bundlefile", service);

  return service;
}

} // namespace axgl
