#pragma once

#include <axgl/axgl.hpp>
#include <axgl/impl/opengl/services/renderer_service.hpp>
#include <axgl/impl/opengl/services/text_service.hpp>

namespace axgl
{

template <>
inline std::shared_ptr<impl::OpenglRendererService> Axgl::use_service()
{
  // register opengl service
  const auto opengl_service = std::make_shared<impl::OpenglRendererService>();
  register_service(DefaultServices::kRenderer, opengl_service);

  // register opengl related services
  // const auto gui_service = std::make_shared<impl::OpenglGuiService>();
  // register_service("gui", std::static_pointer_cast<axgl::GuiService>(gui_service));
  const auto text_service = std::make_shared<impl::OpenglTextService>();
  register_service("text", text_service);

  return opengl_service;
}

} // namespace axgl
