#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/services/gui_service.hpp>
#include <axgl/impl/opengl/services/renderer_service.hpp>
#include <axgl/impl/opengl/services/text_service.hpp>

namespace axgl
{

template <>
inline std::shared_ptr<impl::opengl::RendererService> Axgl::use_service()
{
  const auto service = std::make_shared<impl::opengl::RendererService>();
  register_service(DefaultServices::kRenderer, service);

  return service;
}

template <>
inline std::shared_ptr<impl::opengl::TextService> Axgl::use_service()
{
  const auto service = std::make_shared<impl::opengl::TextService>();
  register_service("text", service);

  return service;
}

template <>
inline std::shared_ptr<impl::opengl::GuiService> Axgl::use_service()
{
  const auto service = std::make_shared<impl::opengl::GuiService>();
  register_service("gui", service);

  return service;
}

} // namespace axgl
