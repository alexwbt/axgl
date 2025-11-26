#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/services/renderer_service.hpp>
// #include <axgl/impl/opengl/services/text_service.hpp>

namespace axgl
{

template <>
inline std::shared_ptr<impl::OpenglRendererService> Axgl::use_service()
{
  const auto opengl_service = std::make_shared<impl::OpenglRendererService>();
  register_service(DefaultServices::kRenderer, opengl_service);

  return opengl_service;
}

} // namespace axgl
