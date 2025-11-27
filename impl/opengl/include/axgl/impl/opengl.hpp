#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/services/renderer_service.hpp>
#include <axgl/impl/opengl/services/text_service.hpp>

namespace axgl
{

template <>
inline std::shared_ptr<impl::OpenglRendererService> Axgl::use_service()
{
  const auto service = std::make_shared<impl::OpenglRendererService>();
  register_service(DefaultServices::kRenderer, service);

  return service;
}

template <>
inline std::shared_ptr<impl::OpenglTextService> Axgl::use_service()
{
  const auto service = std::make_shared<impl::OpenglTextService>();
  register_service("text", service);

  return service;
}

} // namespace axgl
