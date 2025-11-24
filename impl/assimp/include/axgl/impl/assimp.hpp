#pragma once

#include <axgl/axgl.hpp>

/* Service implementations */

#include <axgl/impl/assimp/model.hpp>

namespace axgl
{

template <> inline std::shared_ptr<impl::AssimpModelService> Axgl::use_service()
{
  auto service = std::make_shared<impl::AssimpModelService>();
  register_service("model", service);

  return service;
}

} // namespace axgl
