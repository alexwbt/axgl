#pragma once

#include <axgl/axgl.hpp>

#include <axgl/impl/assimp/model.hpp>

namespace axgl
{

template <>
inline std::shared_ptr<impl::AssimpModelService> Axgl::use_service()
{
  auto service = std::make_shared<impl::AssimpModelService>();
  register_service(DefaultServices::kModel, service);

  return service;
}

} // namespace axgl
