#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/services/bundlefile_service.hpp>

namespace axgl
{

template <>
inline std::shared_ptr<impl::BundlefileService> Axgl::use_service()
{
  auto bundlefile_service = std::make_shared<impl::BundlefileService>();
  register_service("bundlefile", bundlefile_service);

  return bundlefile_service;
}

} // namespace axgl
