#pragma once

#include <axgl/axgl.hpp>

/* Service implementations */

#include <axgl/impl/assimp/model.hpp>

NAMESPACE_AXGL

template<>
std::shared_ptr<impl::AssimpModelService> Axgl::use_service()
{
  auto service = std::make_shared<impl::AssimpModelService>();
  register_service("model", service);

  return service;
}

NAMESPACE_AXGL_END
